#include<bits/stdc++.h>
using namespace std;
//#define int long long int
#define S 100

int p,q,n,t,flag,e[S],d[S],temp[S], m[S], en[S];
string msg;
int plusminus=67;  //prime number


class RSA_Encryption
{
  private:

  void decrypt() 
  {
   int pt, ct, key = d[0], k;
   int i = 0;
   int cnt = 0;
   while (en[i] != -1)
   {
     ct = temp[i];
     k = 1;
     for (int j = 0; j < key; j++)
     {
       k *= ct;
       k %= n;
     }
     pt = k + plusminus;
     m[i] = pt;
     i++;
     cnt++;
  }
  cout << "\nDecrypted message :";
  for (int x = 0; x<cnt; x++) printf("%c", m[x]);
 }

public:
 bool primecheck(int var)
 {
  if (var == 2)return true;
  int i, j = sqrt(var);
  for (i = 2; i <= j; ++i)
  {
    if (var % i == 0)
    {
      return false;
    } 
  }
  return true;
 }

int private_key_generation(int x) 
{
  long int k = 1;
  while (true)
  {
    k += t;
    if (k % x == 0) return (k / x);
  }
}

void public_key_generation()
{
  int k = 0;
  for (int i = 2; i < t; i++)
  {
    if (t % i == 0) continue;
    flag = primecheck(i);
    if (flag == 1 && i != p && i != q)
    {
      e[k] = i;
      flag = private_key_generation(e[k]);
      if (flag > 0)
      {
        d[k] = flag;
      }
      k++;
      if (k == 99)break;
    }
  }
}

void encrypt()
{
  int pt, ct, key = e[0],k, len;
  int i = 0;
  len = msg.size();
  int cnt =0;
  while (i != len)
  {
    pt = m[i];
    pt = pt - plusminus;
    k = 1;
    for (int j = 0; j < key; j++)
    {
      k *= pt;
      k %= n;
    }

    temp[i] = k;
    ct = k + plusminus;
    en[i] = ct;
    cnt++;
    i++;
  }
  en[i]=-1;
  cout << "Encrypted message :";
  for (int j = 0; j<cnt; j++) printf("%c", en[j]);
  decrypt();
 }
};

class Vigenere_Cipher
{
  private:

  int decrypt()
  {
    for(int i=0; i < plaintext.length(); i++) output+=(plaintext[i]-newkey[i]+26)%26 + 65;
    cout<<"Decrypted message:"<<output<<endl;
  }

  public:

  string plaintext, ciphertext, key, output;
  bool choice;
  string newkey = key;
  int encrypt()
  {
     for(int i = 0; i < plaintext.length(); i++) ciphertext +=(plaintext[i]+newkey[i])%26 +65;
     cout<<"Encrypted message:"<<ciphertext<<endl;
     decrypt();
  }
};



int main()
{
  freopen("input.txt","r",stdin);freopen("output8.txt","w",stdout);
   
  //Creating object of class Encrypt
  RSA_Encryption encr;
  Vigenere_Cipher cipher;
  
  int choice;
  cout<<"Enter choice:"<<endl;;
  cout<<"1. Encrypt/Decrypt by RSA algorithm"<<endl;
  cout<<"2. Encrypt/Decrypt by Vigenere Cipher algorithm"<<endl;
  cin>>choice;

  if(choice==1)
  {
    while(1)
    {
    cout << "\nEnter two primes (p,q for Euler's totient to be taken as (p-1)(q-1)) :\n";
    cin >> p >> q;
    if(encr.primecheck(p) && encr.primecheck(q)) break;
    else cout << "Wrong input; enter again (-_-)\n";
    }
    cout << "\nEnter message for encryption :\n\n";
    cin >> msg;
    for (int i = 0; i<msg.size(); i++) m[i] = msg[i];
    n = p * q;
    t = (p - 1) * (q - 1);
    encr.public_key_generation();
    encr.encrypt();
  }
  else if(choice==2)
  {
     cout<<"Enter the plaintext"<<endl;
     cin>>cipher.plaintext;
     cout<<"Enter the key"<<endl;
     cin>>cipher.key;
     while(cipher.newkey.length()<=cipher.plaintext.length()) cipher.newkey+=cipher.key;
     cipher.encrypt();
  }

}