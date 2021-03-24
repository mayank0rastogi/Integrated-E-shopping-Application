#include<bits/stdc++.h>
using namespace std;


int cnt=0;
int positivedata[5001][10000],negativedata[5001][10000],positiveindex=0,negativeindex=0;
int positivesentiment[5001],negativesentiment[5001];
int tokenprob_num[10000][2]; //numerator of the probability of occurrence/non-occurrence of token[i]
int tokenprob_den[10000][2]; //denominator of the probability of occurrence/non-occurrence of token[i]
int prob_givenpositive_num[10000][2]; //numerator of the probability of occurrence/non-occurrence of token[i] given it belongs to positive class.
int prob_givenpositive_den[10000][2]; //denominator of the probability of occurrence/non-occurrence of token[i] given it belongs to positive class.
int prob_givennegative_num[10000][2]; //numerator of the probability of occurrence/non-occurrence of token[i] given it belongs to negative class.
int prob_givennegative_den[10000][2]; //denominator of the probability of occurrence/non-occurrence of token[i] given it belongs to negative class.
bool sentiment[5001],test_sentiment[5001];
char store[32]={'!','"','#','$','%','&','\'','(',')','*','+',',','-','.','/',':',';','<','=','>','?','@','[','\\',']','^','_','`','{','|','}','~',};
string line, stopword, exists;
vector<string>tokens,test_tokens,sentences,testsent;
unordered_map<string,bool>hashes;
unordered_map<string,int>tokenhash,visit;
int ind,token_hash,totalsentences,totaltokens,datasetsize,positiveprob_numerator,positiveprob_denominator,negativeprob_numerator,negativeprob_denominator,true_positive=0,true_negative=0,false_negative=0,false_positive=0,bagofword[10000][10000],punctuation[128]={0};

class Sentiment_Analysis
{
public:

int ischar(char c)
{
  if(c>='a'&& c<='z')return 1;
  if(c>='A'&& c<='Z')return 1;
  return 0;
}

int notchar(string str,int ind)
{
    char c=str[ind-1];
    char d=str[ind+1];
    for(int i=0;i<32;i++)if(str[ind]==store[i])return 1;
    if(ischar(c) && ischar(d))return 0;
  return 1;
}

int clean_data(string &sentence,int i,string check)
{
  for(int j=0;j<sentence.size();j++)
  {
   char word=sentence[j];
   int ascii_word=(int)word;
   int last=sentence.size()-1;

   if(sentence[last]=='p' && check=="train"){sentence[last]=' ';sentiment[i]=1;}
   if(sentence[last]=='n' && check=="train"){sentence[last]=' ';sentiment[i]=0;}

   if(sentence[last]=='p' && check=="test"){sentence[last]=' ';test_sentiment[i]=1;  }
   if(sentence[last]=='n' && check=="test"){sentence[last]=' ';test_sentiment[i]=0;}

   if(punctuation[ascii_word]==1 && notchar(sentence,j) )sentence[j]=' ';
   if(ascii_word >= 48 && ascii_word <= 57)sentence[j]=' ';
   if(sentence[j] >= 65 && sentence[j] <=90)
   sentence[j]=97+sentence[j]-65;  
  }

}

char tokenize_test(string &line)
{
  stringstream tkn(line);
  string token;
  test_tokens.clear();
  visit.clear();
  while(tkn>>token)
  {
   //check if it's a stopword or not
    if(hashes[token]==0 && visit[token]==0) // If it's not a stopword and if its a new token,then save it.
    {
      test_tokens.push_back(token); //saving unique tokens
      visit[token]=1;
    }
  }
  cnt++;
}

int tokenize_train(string &line)
{
  stringstream tkn(line);
  string token;
  
  while(tkn>>token)
  {
   //check if it's a stopword or not
    if(hashes[token]==0 && tokenhash[token]==0) // If it's not a stopword and if its a new token,then save it.
    {
      tokens.push_back(token); //saving unique tokens
      tokenhash[token]=ind++; //hashing the token
      if(tokenhash[token]==0)exists=token;
      token_hash=tokenhash[token];
      bagofword[cnt][token_hash]=1; //forming feature matrix  
    }
    else if(hashes[token]==0 && tokenhash[token]!=0)// If it's not a stopword and if its not a new token,then increment it.
    {
      token_hash=tokenhash[token];
      bagofword[cnt][token_hash]+=1; //forming feature matrix  
    }
  }
  cnt++;
}

int laplace_smoothing(int *num, int *den,string check)
{
  int alpha=1,number_of_class=2;
  *num=0+alpha;
  *den= alpha * number_of_class;
  
  if(check=="positive")*den+=positiveindex;
  else if(check=="negative")*den+=negativeindex;
}

int display_sentiment(string &line)
{ 
 long double positive_sent_num_num=positiveprob_numerator;
 long double positive_sent_num_den=positiveprob_denominator;

 // int positive_sent_den_num=1;
 // int positive_sent_den_den=1;
 long double  negative_sent_num_num=negativeprob_numerator;
 long  double negative_sent_num_den=negativeprob_denominator;

 int turn =0;
 //positive probability testing
 for(auto x: test_tokens)
 {
  int i=tokenhash[x];
  int num=0;
  int den=0;
  if((i==0 && x!=exists) || prob_givenpositive_num[i][1]==0) //This means, this token is a new word
  {
    laplace_smoothing(&num,&den,"positive");
  }
  else 
  {
     num=prob_givenpositive_num[i][1];
     den=prob_givenpositive_den[i][1];
  }
  positive_sent_num_num*=num;
  positive_sent_num_num/=den;
 // cout<<positive_sent_num_num<<endl;
 } 

//if(positive_sent_num_num==0)cout<<"hi"<<endl;

//Negative probability testing
for(auto x: test_tokens)
{
  int i=tokenhash[x];
  int num;
  int den;
  if((i==0 && x!=exists) || prob_givennegative_num[i][1]==0) //This means, this token is a new word
  {
    laplace_smoothing(&num,&den,"negative");
  }
  else 
  {
     num=prob_givennegative_num[i][1];
     den=prob_givennegative_den[i][1];
  }
 
  
  negative_sent_num_num*=num;
  negative_sent_num_num/=den;
} 
//Comparing positive and negative probabilities by cross multiplication 
//cross multiplication is done to avoid precision issues!

float positive_ans=positive_sent_num_num ;
float negative_ans=negative_sent_num_num ;

if(positive_ans > negative_ans )
  {
    //cout<<"This review is positive!"<<endl;
    return 1;
  }
else if(positive_ans == negative_ans)
  {
    //cout<<"This review is neutral!"<<endl;
    return -1;
  }
else if(positive_ans < negative_ans)
  {
    //cout<<"This review is negative!"<<endl;
    return 0;
  }
}

};

int main()
{
 Sentiment_Analysis sent;
 freopen("Training_data_phone reviews.txt","r",stdin);
 freopen("output8.txt","w",stdout);

 //Marking the punctuations
 for(int i=0;i<32;i++)
 {
  char p=store[i];
  punctuation[(int)p]=1; 
 }

 //Inputting and storing the sentences
 while(getline(cin,line))
 {
  sentences.push_back(line);
 }
 
 totalsentences = sentences.size();

//-------------------------------------------------------------------------------
//Remove punctuation, numbers from sentences and lowering the case

 for(int i=0;i<sentences.size();i++)
 {
    sent.clean_data(sentences[i],i,"train");
 }
 
//------------------------------------------------------------------------
 //Marking stopwords through string hashing
  ifstream fin;
  ofstream fout;
  fin.open("stopwords.txt");
 
  cnt=0;
  while(fin)
  {
    getline(fin,stopword);
    hashes[stopword]=1;
  }
  cnt=0,ind=0;
  
//--------------------------------------------------------------------------------
//Performing tokenization and not saving the tokens which are stopwords
//BAG OF WORDS : Making the feature matrix
 for(auto line: sentences)
 {
   sent.tokenize_train(line);  
 }
 totaltokens=tokens.size();
//-----------------------------------------------------------
// Step 1,2,3:
//Splitting the dataset into two classes : Positive and negative
//1.Taking the Positive class.

for(int i=0;i<totalsentences;i++)
{
  if(sentiment[i]==1)
  {
    for(int j=0;j<totaltokens;j++)
    {
      positivedata[positiveindex][j]=bagofword[i][j];
    }
    positivesentiment[positiveindex]=sentiment[i];
    positiveindex++;
  }
  else if(sentiment[i]==0)
  {
    for(int j=0;j<totaltokens;j++)
    {
      negativedata[negativeindex][j]=bagofword[i][j];
    }
    negativesentiment[negativeindex]=sentiment[i];
    negativeindex++;
  }

}
datasetsize=positiveindex + negativeindex;
//-------------------------------------------------
//Step 4: 
//Calculate probability of Positive class

/*Formula used :    positiveindex
                --------------------- 
                     datasetsize               
*/

positiveprob_numerator=positiveindex;
positiveprob_denominator=datasetsize;
//-------------------------------------------------
//Step 5:
//Calculate probability of Negative class

/*Formula used :    negativeindex
                --------------------- 
                     datasetsize               
*/

negativeprob_numerator=negativeindex;
negativeprob_denominator=datasetsize;

//--------------------------------------------------------
//Step 6 and 7: 
//Probability of occurrence and non-occurrence of each token in the dataset.
//Occurrence is denoted by 1. (bagofword[sentence_number][token_number]=1)
//Non-Occurrence is denoted by 0. (bagofword[sentence_number][token_number]=0)

/*
Probability of occurrence of token[i] in dataset: 
Count 1s of token[i] in positivedata + Count 1s of token[i] in negativedata  
  ----------------------------------------------------------------------------
                                    totaldatasetsize
Probability of non-occurrence of token[i] in dataset:
datasetsize - Count 1s of token[i] in positivedata + Count 1s of token[i] in negativedata  
-----------------------------------------------------------------------------------------
                                    totaldatasetsize 
*/

/*
Step 8:
Probability of token[i],given that it's class is positive.
Denoted in coding as prob_givenpositive[token[i]][occurence] 
Represented as P( Token[i] | Positive ) = 
                                  No. of 1s of token[i]
prob_givenpositive[token[i]][1]  ---------------------
                                  positivedatasetsize
                                 No. of 0s of token[i]
prob_givenpositive[token[i]][0]  ---------------------
                                  positivedatasetsize
                                  No. of 1s of token[i]
prob_givennegative[token[i]][1]  ---------------------
                                  negativedatasetsize
                                 No. of 0s of token[i]
prob_givennegative[token[i]][0]  ---------------------
                                  negativedatasetsize
Note : This step will be done in between step 6 and 7.
*/


//Counting 1s of token[i] in positive dataset and negative dataset

for(int i=0;i<totaltokens;i++)
{
  prob_givenpositive_num[i][0]=0;
  prob_givenpositive_num[i][1]=0;

  prob_givennegative_num[i][0]=0;
  prob_givennegative_num[i][1]=0;
}

for(int i=0;i<totaltokens;i++) //Iterating over all tokens
{ 
   int ones=0;
   tokenprob_num[i][1]=0;
   tokenprob_num[i][0]=0;

   //Counting 1s of token[i] in positive dataset
   for(int j=0;j<positiveindex;j++)
   {
      if(positivedata[j][i]==1)ones++;
   }

   tokenprob_num[i][1]+=ones;

   prob_givenpositive_num[i][1]=ones;
   prob_givenpositive_den[i][1]=positiveindex;

   prob_givenpositive_num[i][0]=positiveindex-ones; //zeros = negativeindex-ones;
   prob_givenpositive_den[i][0]=positiveindex;

   ones=0; //Most important line.because we are using this variable again for counting in another dataset

   //Counting 1s of token[i] in negative dataset
   for(int j=0;j<negativeindex;j++)
   {
      if(negativedata[j][i]==1)ones++;
   }
  
   prob_givennegative_num[i][1]=ones;
   prob_givennegative_den[i][1]=negativeindex;

   prob_givennegative_num[i][0]=negativeindex-ones; //zeros = negativeindex-ones;
   prob_givennegative_den[i][0]=negativeindex;
   
   tokenprob_num[i][1]+=ones; //contains count of all occurrences of token[i]
   tokenprob_den[i][1]=datasetsize;

   tokenprob_num[i][0]=datasetsize- tokenprob_num[i][1]; //zeros= datasetsize- tokenprob_num[i][1];
   tokenprob_den[i][0]=datasetsize;

  //Count 0s of token[i] in positivedata + Count 0s of token[i] in negativedata = datasetsize - sum
  //Rest of the elements of token[i] except 1s are 0s.
}
//--------------------------------------------------------------------------------------
//Testing our Naive bayes model on unseen data

ifstream fiin;
fiin.open("Test_data_phone reviews.txt");
int cn=0;
while(getline(fiin,line))
{
  testsent.push_back(line);
  cn++;
}
cout<<"Total_data="<<cn+datasetsize<<endl;
cout<<"Testing data="<<cn<<" "<<"(20%)"<<endl;
cout<<"Training data="<<datasetsize<<"(80%)"<<endl;
for(int i=0;i<cn;i++)
{
  sent.clean_data(testsent[i],i,"test");
  sent.tokenize_test(testsent[i]);
  int polarity=sent.display_sentiment(testsent[i]);

  if(test_sentiment[i]==1)
  {
    if(polarity==1)true_positive++;
    else if(polarity==0)false_negative++;
  }
  else if(test_sentiment[i]==0)
  {
    if(polarity==0)true_negative++;
    else if(polarity==1)false_positive++;
  }
}

  float accuracy = true_negative + true_positive;
  accuracy/=true_negative + true_positive + false_positive + false_negative;

  cout<<"Accuracy of Naive bayes model is "<<accuracy*100<<"%"<<endl;
//----------------------------------------------------------------------------------
}