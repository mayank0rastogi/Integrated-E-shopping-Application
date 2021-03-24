#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include<unordered_map>
#include<vector>
#include <algorithm>
#include<cmath>
using namespace std;

//user-defined variable 
struct phone_rate
{
  int phone_id;
  float rating;
};

//function to compare between two phone
bool compare_phone_rating_score(const phone_rate& lhs,const phone_rate& rhs){
  return lhs.rating>rhs.rating;
}


unordered_map<int, unordered_map<int, float>> rating_map;
unordered_map<int, vector<int>> phone_user;
unordered_map<int, float> avg_rating;
unordered_map<int, string> phone_map;

unordered_map<int, vector<phone_rate>> database;


class RecommendationSystem
{

 public:

//this function is to read the phone models name file
bool read_phone_title(string filename) 
{
  freopen(filename.c_str(),"r",stdin);
  
  int id_int;
  string ID,  title;
  
  while (1)
  {
  
    cin>>id_int;    
    if(id_int==-1)break;
    getline(cin,title);
    phone_map[id_int] = title;
  }
  return true;
}

//function to read the rating file
bool read_rating(string filename)
 {
    freopen(filename.c_str(),"r",stdin);
    int user_id, phone_id;
    float rating;
    string phone_id_str, user_id_str, rating_str;
    while (1)
    {
      cin>>phone_id>>user_id>>rating;
      if(phone_id==-1){break;}
      rating_map[user_id][phone_id]=rating; //rating_map
      phone_user[phone_id].push_back(user_id); //phone_id->user_id
    }

  return true;
}

//function to find the user average rating
void user_rating_avg()
{
  for (auto& user_it : rating_map)
  {
    float sum=0;
    int count = 0;
    for (auto& phone_user_it : user_it.second)
    {
      sum +=phone_user_it.second;//summing the rating of the users 
      ++count;//count the total no of rating 
    }
    avg_rating[user_it.first]=sum/count;//finding the average of the rating
  }
}

unordered_map<int, float> get_correlation(int active_user)///function to get correlation values
{
  unordered_map<int, float>corr_map;


  unordered_map<int, float>& active_user_phone_rating=rating_map[active_user];
  ///this unordered map will store a phone_id  for the particular active_user 
  ///finding inside the rating_map 

  


  for(auto& user_it: rating_map)
        {
    int user_id=user_it.first; //from the rating_map store the user_id

    if(user_id==active_user) ///checks whether user_id equals to active_user than start 
      //from the beginning 
      continue;
     //if not equals than insert all ratings and movie_id in the user_phone_rating of  
     //other users except the active_users ratings

    unordered_map<int, float>& user_phone_rating=user_it.second;
 
   // Formula ~ Cxy=Σ(Xi-mean(Xi))*(Yi-mean(Yi))/sqrt(Σ ((Xi-mean(Xi))^2) * Σ ((Yi-mean(Yi))^2))
    
    float numerator = 0;  
    float sum_vaj_diff = 0;
    float sum_vij_diff = 0;
         
    for(auto& phone_it: active_user_phone_rating)
        {
      int phone_id = phone_it.first;//storing the movie_id
           
      if(user_phone_rating.find(phone_id) == user_phone_rating.end())
         continue;
     //this if statement traverse thorugh all the movie_id of the active_user_phone_rating 
     // if it is found inside the user_movie_rate  than skip and start the checking again
     //if not than find on remaining other user_movie_rate on which i have applied correlation  
     //formula to find the corrrelation values of other users
      // numerator = Σ(Xi-mean(Xi))*(Yi-mean(Yi))
      numerator +=  (rating_map[active_user][phone_id] - avg_rating[active_user]) * (rating_map[user_id][phone_id] - avg_rating[user_id]);
      
      // sum_vaj_diff = Σ ((Xi-mean(Xi))^2)
      
      sum_vaj_diff +=  pow(rating_map[active_user][phone_id] - avg_rating[active_user], 2);
       
       // sum_vij_diff = Σ ((Yi-mean(Yi))^2)

      sum_vij_diff +=  pow(rating_map[user_id][phone_id] - avg_rating[user_id], 2);
         
          
    }
          //denominator = sqrt(Σ ((Xi-mean(Xi))^2) * Σ ((Yi-mean(Yi))^2))
        float denominator= sqrt(sum_vaj_diff * sum_vij_diff);
        if(denominator!=0)
          //contains correlation value of a every user_id
          corr_map[user_id] = numerator/denominator;
  }
  
  return corr_map;
}


void phone_recommendation_system(int active_user, int K)
{

  vector<phone_rate>* predicted_rating=NULL;//pointer variable
    float pred_rating = 0;
    if(database.find(active_user)==database.end())
        {
   //corr_active unordered_map contains the correlation value of the other user 
   // following their user_id except active_user
    unordered_map<int, float> corr_active = get_correlation(active_user);
    
    predicted_rating=new vector<phone_rate>();//assigning dynamically the memory to phone_rate structure

        

    for(auto& phone_it : phone_map)//iterate over all phone_id
            {
                pred_rating=0;


      int phone_id=phone_it.first;

      if(phone_user.find(phone_id)!=phone_user.end())
                {

        vector<int>& user_list=phone_user[phone_id];
       

        for(int user_id : user_list)
                    {

          if(corr_active.find(user_id) == corr_active.end())///if search not found that it starts from beginning
            continue;
          //here we check if we find the all phone-id of the phone_map
          //inside all users who have rate the phone which is inside the ratings_review only that users
          //we have store inside user_list unordered_map

         ///final predicted rating by finding the correlation of a particular user_id
          pred_rating +=corr_active[user_id]*(rating_map[user_id][phone_id]- avg_rating[user_id]);
         
        }

      }


     //initialize the object of the phone_rate structure and using that object 
      //i have to acces the data inside the phone_rate vector
      phone_rate m;
      m.phone_id=phone_id;
      m.rating=pred_rating;
     //now push the data into predicted_rating vector pointer
      predicted_rating->push_back(m);

    }
    //now sort the value using compare_phone_rating_score
    sort(predicted_rating->begin(), predicted_rating->end(),compare_phone_rating_score);

    database[active_user]=*predicted_rating;

    corr_active.clear();
}
  else
    {
      //if not find the active user gives the active user rating
    predicted_rating=&database[active_user];
  }

  cout<<"\n"<<K <<" recommended phone models for user : "<<active_user<<"\n";

  for(int i=0;i<K;++i)
    {
        cout<<phone_map[predicted_rating->at(i).phone_id]<<"->" <<predicted_rating->at(i).rating<<"\n";

    }


}

};

int main()
{
  
  RecommendationSystem obj;
  string phone_title_file="Mobiles_list.txt";
  string rating_file="ratings_without_reviews.txt";

  cout<< "1.Reading file \""<<phone_title_file<<"\" ...\n\n\n";
  obj.read_phone_title(phone_title_file);
  cout<<phone_title_file<<" "<<"processed successfully!"<<endl;


  cout<<"\n2.Reading file \""<<rating_file<<"\" ...\n\n\n";
  obj.read_rating(rating_file);
  cout<<rating_file<<" "<<"processed successfully!"<<endl;
  

  ifstream fin;
  fin.open("input.txt");
  freopen("output8.txt","w",stdout);
  cout<<"\nComputing User average rating.... \n";
  obj.user_rating_avg();
  int user_id=-2, K;
  while(true)
  {
    cout<<"Please Enter UserID (or enter -1 to exit): "; 
    fin>>user_id; 

    if(user_id==-1)
    {
      break;
    }


    if(rating_map.find(user_id)==rating_map.end())
    {
      cout<<"UserID not found, Please enter valid UserID\n";
    }
    else
      {
      cout<<"Please Enter Number of Recommendation: ";
      fin>>K;
      if(K>0)
      {
        cout<<"Calling phone_recommendation_system(user_id, K)"<<endl;
        obj.phone_recommendation_system(user_id, K);
      }
      else
      {
        cout<<"Number of recommendation must be greater than 0\n";
      }
       }

    cout<<"\n";
  }   
  return 0;
}