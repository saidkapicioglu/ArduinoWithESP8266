#define ag_ismi "itYSR"
#define ag_sifresi "86cCe50EE7988"
#define server "pusheriot.azurewebsites.net"    
#define baslik "sulamabildirimi"
#define mesaj "sulamabaslamistir"
#include "DHT.h"
#include "fis_header.h"
#define DHTPIN 2

#define DHTTYPE DHT11
// İnputlar Sayısı
const int fis_gcI = 3;
// Number of outputs to the fuzzy inference system
const int fis_gcO = 1;
// Number of rules to the fuzzy inference system
const int fis_gcR = 135;

FIS_TYPE g_fisInput[fis_gcI];
FIS_TYPE g_fisOutput[fis_gcO];

 
DHT meraklimuhendis(DHTPIN, DHTTYPE);
int sicaklik;
const int toprak=A0;
int olcum_sonucu;
void setup()
{
  
  Serial.begin(115200); //Seriport'u açıyoruz. Güncellediğimiz 
                        //ESP modülünün baudRate değeri 115200 olduğu için bizde Seriport'u 115200 şeklinde seçiyoruz
  pinMode(0 , INPUT);
    // Pin mode for Input: Tnem
    pinMode(1 , INPUT);
    // Pin mode for Input: Donemler
    pinMode(2 , INPUT);


    // initialize the Analog pins for output.
    // Pin mode for Output: SuMiktari
    pinMode(3 , OUTPUT);
  Serial.println("AT"); //ESP modülümüz ile bağlantı kurulup kurulmadığını kontrol ediyoruz.

  delay(4000); //ESP ile iletişim için 3 saniye bekliyoruz.
  analogReference(DEFAULT);
  if(Serial.find("OK")){         //esp modülü ile bağlantıyı kurabilmişsek modül "AT" komutuna "OK" komutu ile geri dönüş yapıyor.
     Serial.println("AT+CWMODE=1"); //esp modülümüzün WiFi modunu STA şekline getiriyoruz. Bu mod ile modülümüz başka ağlara bağlanabilecek.
     delay(2000);
     String baglantiKomutu=String("AT+CWJAP=\"")+ag_ismi+"\",\""+ag_sifresi+"\"";
    Serial.println(baglantiKomutu);
    
     delay(3000);
     meraklimuhendis.begin();
 }
}
 
void loop(){
  delay(1000);
  olcum_sonucu=analogRead(toprak);
  olcum_sonucu= map(olcum_sonucu,0,1023,100,0);
  int sicaklik = meraklimuhendis.readTemperature();
  Serial.print("Sicaklik = ");
  Serial.println(sicaklik);
  Serial.print("Toprak Nemi = ");
  Serial.println(olcum_sonucu);
  yolla(sicaklik,olcum_sonucu);

  // Sicaklik
  g_fisInput[0] = sicaklik;
  // Nem
  g_fisInput[1] = olcum_sonucu;
  g_fisInput[2] = 5;
  g_fisOutput[0] = 0;
  
  fis_evaluate();
  analogWrite(3 , g_fisOutput[0]);
    
  Serial.println(g_fisInput[0]);
 
  Serial.println(g_fisInput[1]);
  Serial.println(g_fisInput[2]);
 
  Serial.println(g_fisOutput[0]);
  notf(g_fisOutput[0]);
  
  delay(8000);
  
}
void yolla(int sicaklik,int toprak_nemi){
  Serial.println(String("AT+CIPSTART=\"TCP\",\"") + server + "\",80");  //thingspeak sunucusuna bağlanmak için bu kodu kullanıyoruz. 
                                                                     //AT+CIPSTART komutu ile sunucuya bağlanmak için sunucudan izin istiyoruz. 
                                                                     //TCP burada yapacağımız bağlantı çeşidini gösteriyor. 80 ise bağlanacağımız portu gösteriyor
  delay(1000);
  if(Serial.find("Error")){     //sunucuya bağlanamazsak ESP modülü bize "Error" komutu ile dönüyor.
   Serial.println("AT+CIPSTART Error");
    return;
  }
  String yollanacakkomut = "GET /kayit/556644/demo/"+String(sicaklik)+"/"+String(toprak_nemi)+" HTTP/1.1\r\n";// Burada ise sıcaklığımızı float değişkenine atayarak yollanacakkomut değişkenine ekliyoruz.
  yollanacakkomut +="Host: pusheriot.azurewebsites.net\r\n";
  yollanacakkomut +="Connection: close";
  yollanacakkomut += "\r\n\r\n";                                             // ESP modülümüz ile seri iletişim kurarken yazdığımız komutların modüle iletilebilmesi için Enter komutu yani
  delay(500);
  Serial.print("AT+CIPSEND=");                    //veri yollayacağımız zaman bu komutu kullanıyoruz. Bu komut ile önce kaç tane karakter yollayacağımızı söylememiz gerekiyor.
  Serial.println(yollanacakkomut.length()+2);       //yollanacakkomut değişkeninin kaç karakterden oluştuğunu .length() ile bulup yazırıyoruz.
  
  delay(1000);
  
  if(Serial.find(">")){                           //eğer sunucu ile iletişim sağlayıp komut uzunluğunu gönderebilmişsek ESP modülü bize ">" işareti ile geri dönüyor.
                                                  // arduino da ">" işaretini gördüğü anda sıcaklık verisini esp modülü ile thingspeak sunucusuna yolluyor.
  Serial.print(yollanacakkomut);
  Serial.print("\r\n\r\n");
  }
  else{
  Serial.println("AT+CIPCLOSE");
  }
  delay(3000);
}

void notf(int output){
  Serial.println(String("AT+CIPSTART=\"TCP\",\"") + server + "\",80");  //thingspeak sunucusuna bağlanmak için bu kodu kullanıyoruz. 
                                                                     //AT+CIPSTART komutu ile sunucuya bağlanmak için sunucudan izin istiyoruz. 
                                                                     //TCP burada yapacağımız bağlantı çeşidini gösteriyor. 80 ise bağlanacağımız portu gösteriyor
  delay(1000);
  if(Serial.find("Error")){     //sunucuya bağlanamazsak ESP modülü bize "Error" komutu ile dönüyor.
   Serial.println("AT+CIPSTART Error");
    return;
  }
  String yollanacakkomut = "GET /Not/556644/"+String(output)+" HTTP/1.1\r\n";                                    // Burada ise sıcaklığımızı float değişkenine atayarak yollanacakkomut değişkenine ekliyoruz.
  yollanacakkomut +="Host: pusheriot.azurewebsites.net\r\n";
  yollanacakkomut +="Connection: close";
  yollanacakkomut += "\r\n\r\n";                                             // ESP modülümüz ile seri iletişim kurarken yazdığımız komutların modüle iletilebilmesi için Enter komutu yani
  delay(500);
  Serial.print("AT+CIPSEND=");                    //veri yollayacağımız zaman bu komutu kullanıyoruz. Bu komut ile önce kaç tane karakter yollayacağımızı söylememiz gerekiyor.
  Serial.println(yollanacakkomut.length()+2);       //yollanacakkomut değişkeninin kaç karakterden oluştuğunu .length() ile bulup yazırıyoruz.
  
  delay(1000);
  
  if(Serial.find(">")){                           //eğer sunucu ile iletişim sağlayıp komut uzunluğunu gönderebilmişsek ESP modülü bize ">" işareti ile geri dönüyor.
                                                  // arduino da ">" işaretini gördüğü anda sıcaklık verisini esp modülü ile thingspeak sunucusuna yolluyor.
  Serial.print(yollanacakkomut);
  Serial.print("\r\n\r\n");
  }
  else{
  Serial.println("AT+CIPCLOSE");
  }
  delay(3000);
}


//***********************************************************************
// Support functions for Fuzzy Inference System                          
//***********************************************************************
// Triangular Member Function
FIS_TYPE fis_trimf(FIS_TYPE x, FIS_TYPE* p)
{
    FIS_TYPE a = p[0], b = p[1], c = p[2];
    FIS_TYPE t1 = (x - a) / (b - a);
    FIS_TYPE t2 = (c - x) / (c - b);
    if ((a == b) && (b == c)) return (FIS_TYPE) (x == a);
    if (a == b) return (FIS_TYPE) (t2*(b <= x)*(x <= c));
    if (b == c) return (FIS_TYPE) (t1*(a <= x)*(x <= b));
    t1 = min(t1, t2);
    return (FIS_TYPE) max(t1, 0);
}

FIS_TYPE fis_min(FIS_TYPE a, FIS_TYPE b)
{
    return min(a, b);
}

FIS_TYPE fis_max(FIS_TYPE a, FIS_TYPE b)
{
    return max(a, b);
}

FIS_TYPE fis_array_operation(FIS_TYPE *array, int size, _FIS_ARR_OP pfnOp)
{
    int i;
    FIS_TYPE ret = 0;

    if (size == 0) return ret;
    if (size == 1) return array[0];

    ret = array[0];
    for (i = 1; i < size; i++)
    {
        ret = (*pfnOp)(ret, array[i]);
    }

    return ret;
}


//***********************************************************************
// Data for Fuzzy Inference System                                       
//***********************************************************************
// Pointers to the implementations of member functions
_FIS_MF fis_gMF[] =
{
    fis_trimf
};

// Count of member function for each Input
int fis_gIMFCount[] = { 3, 3, 15 };

// Count of member function for each Output 
int fis_gOMFCount[] = { 14 };

// Coefficients for the Input Member Functions
FIS_TYPE fis_gMFI0Coeff1[] = { 2, 8, 15 };
FIS_TYPE fis_gMFI0Coeff2[] = { 12, 16, 20 };
FIS_TYPE fis_gMFI0Coeff3[] = { 18, 25, 35 };
FIS_TYPE* fis_gMFI0Coeff[] = { fis_gMFI0Coeff1, fis_gMFI0Coeff2, fis_gMFI0Coeff3 };
FIS_TYPE fis_gMFI1Coeff1[] = { 0, 15, 30 };
FIS_TYPE fis_gMFI1Coeff2[] = { 25, 45, 60 };
FIS_TYPE fis_gMFI1Coeff3[] = { 55, 75, 100 };
FIS_TYPE* fis_gMFI1Coeff[] = { fis_gMFI1Coeff1, fis_gMFI1Coeff2, fis_gMFI1Coeff3 };
FIS_TYPE fis_gMFI2Coeff1[] = { 0.5, 1, 1.5 };
FIS_TYPE fis_gMFI2Coeff2[] = { 2.5, 3, 3.5 };
FIS_TYPE fis_gMFI2Coeff3[] = { 3.5, 4, 4.5 };
FIS_TYPE fis_gMFI2Coeff4[] = { 4.5, 5, 5.5 };
FIS_TYPE fis_gMFI2Coeff5[] = { 5.5, 6, 6.5 };
FIS_TYPE fis_gMFI2Coeff6[] = { 6.5, 7, 7.5 };
FIS_TYPE fis_gMFI2Coeff7[] = { 7.5, 8, 8.5 };
FIS_TYPE fis_gMFI2Coeff8[] = { 8.5, 9, 9.5 };
FIS_TYPE fis_gMFI2Coeff9[] = { 9.5, 10, 10.5 };
FIS_TYPE fis_gMFI2Coeff10[] = { 10.5, 11, 11.5 };
FIS_TYPE fis_gMFI2Coeff11[] = { 11.5, 12, 12.5 };
FIS_TYPE fis_gMFI2Coeff12[] = { 12.5, 13, 13.5 };
FIS_TYPE fis_gMFI2Coeff13[] = { 13.5, 14, 14.5 };
FIS_TYPE fis_gMFI2Coeff14[] = { 14.5, 15, 15.5 };
FIS_TYPE fis_gMFI2Coeff15[] = { 1.5, 2, 2.5 };
FIS_TYPE* fis_gMFI2Coeff[] = { fis_gMFI2Coeff1, fis_gMFI2Coeff2, fis_gMFI2Coeff3, fis_gMFI2Coeff4, fis_gMFI2Coeff5, fis_gMFI2Coeff6, fis_gMFI2Coeff7, fis_gMFI2Coeff8, fis_gMFI2Coeff9, fis_gMFI2Coeff10, fis_gMFI2Coeff11, fis_gMFI2Coeff12, fis_gMFI2Coeff13, fis_gMFI2Coeff14, fis_gMFI2Coeff15 };
FIS_TYPE** fis_gMFICoeff[] = { fis_gMFI0Coeff, fis_gMFI1Coeff, fis_gMFI2Coeff };

// Coefficients for the Output Member Functions
FIS_TYPE fis_gMFO0Coeff1[] = { 0, 2, 5 };
FIS_TYPE fis_gMFO0Coeff2[] = { 3, 12, 15 };
FIS_TYPE fis_gMFO0Coeff3[] = { 13, 17, 20 };
FIS_TYPE fis_gMFO0Coeff4[] = { 18, 30, 40 };
FIS_TYPE fis_gMFO0Coeff5[] = { 35, 48, 60 };
FIS_TYPE fis_gMFO0Coeff6[] = { 55, 68, 80 };
FIS_TYPE fis_gMFO0Coeff7[] = { 75, 88, 100 };
FIS_TYPE fis_gMFO0Coeff8[] = { 95, 108, 120 };
FIS_TYPE fis_gMFO0Coeff9[] = { 115, 128, 140 };
FIS_TYPE fis_gMFO0Coeff10[] = { 135, 148, 160 };
FIS_TYPE fis_gMFO0Coeff11[] = { 155, 168, 180 };
FIS_TYPE fis_gMFO0Coeff12[] = { 175, 188, 200 };
FIS_TYPE fis_gMFO0Coeff13[] = { 195, 208, 220 };
FIS_TYPE fis_gMFO0Coeff14[] = { 215, 233, 250 };
FIS_TYPE* fis_gMFO0Coeff[] = { fis_gMFO0Coeff1, fis_gMFO0Coeff2, fis_gMFO0Coeff3, fis_gMFO0Coeff4, fis_gMFO0Coeff5, fis_gMFO0Coeff6, fis_gMFO0Coeff7, fis_gMFO0Coeff8, fis_gMFO0Coeff9, fis_gMFO0Coeff10, fis_gMFO0Coeff11, fis_gMFO0Coeff12, fis_gMFO0Coeff13, fis_gMFO0Coeff14 };
FIS_TYPE** fis_gMFOCoeff[] = { fis_gMFO0Coeff };

// Input membership function set
int fis_gMFI0[] = { 0, 0, 0 };
int fis_gMFI1[] = { 0, 0, 0 };
int fis_gMFI2[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int* fis_gMFI[] = { fis_gMFI0, fis_gMFI1, fis_gMFI2};

// Output membership function set
int fis_gMFO0[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int* fis_gMFO[] = { fis_gMFO0};

// Rule Weights
FIS_TYPE fis_gRWeight[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

// Rule Type
int fis_gRType[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

// Rule Inputs
int fis_gRI0[] = { 1, 1, 1 };
int fis_gRI1[] = { 1, 2, 1 };
int fis_gRI2[] = { 1, 3, 1 };
int fis_gRI3[] = { 2, 1, 1 };
int fis_gRI4[] = { 2, 2, 1 };
int fis_gRI5[] = { 2, 3, 1 };
int fis_gRI6[] = { 3, 1, 1 };
int fis_gRI7[] = { 3, 2, 1 };
int fis_gRI8[] = { 3, 3, 1 };
int fis_gRI9[] = { 1, 1, 15 };
int fis_gRI10[] = { 1, 2, 15 };
int fis_gRI11[] = { 1, 3, 15 };
int fis_gRI12[] = { 2, 1, 15 };
int fis_gRI13[] = { 2, 2, 15 };
int fis_gRI14[] = { 2, 3, 15 };
int fis_gRI15[] = { 3, 1, 15 };
int fis_gRI16[] = { 3, 2, 15 };
int fis_gRI17[] = { 3, 3, 15 };
int fis_gRI18[] = { 1, 1, 2 };
int fis_gRI19[] = { 1, 2, 2 };
int fis_gRI20[] = { 1, 3, 2 };
int fis_gRI21[] = { 2, 1, 2 };
int fis_gRI22[] = { 2, 2, 2 };
int fis_gRI23[] = { 2, 3, 2 };
int fis_gRI24[] = { 3, 1, 2 };
int fis_gRI25[] = { 3, 2, 2 };
int fis_gRI26[] = { 3, 3, 2 };
int fis_gRI27[] = { 1, 1, 3 };
int fis_gRI28[] = { 1, 2, 3 };
int fis_gRI29[] = { 1, 3, 3 };
int fis_gRI30[] = { 2, 1, 3 };
int fis_gRI31[] = { 2, 2, 3 };
int fis_gRI32[] = { 2, 3, 3 };
int fis_gRI33[] = { 3, 1, 3 };
int fis_gRI34[] = { 3, 2, 3 };
int fis_gRI35[] = { 3, 3, 3 };
int fis_gRI36[] = { 1, 1, 4 };
int fis_gRI37[] = { 1, 2, 4 };
int fis_gRI38[] = { 1, 3, 4 };
int fis_gRI39[] = { 2, 1, 4 };
int fis_gRI40[] = { 2, 2, 4 };
int fis_gRI41[] = { 2, 3, 4 };
int fis_gRI42[] = { 3, 1, 4 };
int fis_gRI43[] = { 3, 2, 4 };
int fis_gRI44[] = { 3, 3, 4 };
int fis_gRI45[] = { 1, 1, 5 };
int fis_gRI46[] = { 1, 2, 5 };
int fis_gRI47[] = { 1, 3, 5 };
int fis_gRI48[] = { 2, 1, 5 };
int fis_gRI49[] = { 2, 2, 5 };
int fis_gRI50[] = { 2, 3, 5 };
int fis_gRI51[] = { 3, 1, 5 };
int fis_gRI52[] = { 3, 2, 5 };
int fis_gRI53[] = { 3, 3, 5 };
int fis_gRI54[] = { 1, 1, 6 };
int fis_gRI55[] = { 1, 2, 6 };
int fis_gRI56[] = { 1, 3, 6 };
int fis_gRI57[] = { 2, 1, 6 };
int fis_gRI58[] = { 2, 2, 6 };
int fis_gRI59[] = { 2, 3, 6 };
int fis_gRI60[] = { 3, 1, 6 };
int fis_gRI61[] = { 3, 2, 6 };
int fis_gRI62[] = { 3, 3, 6 };
int fis_gRI63[] = { 1, 1, 7 };
int fis_gRI64[] = { 1, 2, 7 };
int fis_gRI65[] = { 1, 3, 7 };
int fis_gRI66[] = { 2, 1, 7 };
int fis_gRI67[] = { 2, 2, 7 };
int fis_gRI68[] = { 2, 3, 7 };
int fis_gRI69[] = { 3, 1, 7 };
int fis_gRI70[] = { 3, 2, 7 };
int fis_gRI71[] = { 3, 3, 7 };
int fis_gRI72[] = { 1, 1, 8 };
int fis_gRI73[] = { 1, 2, 8 };
int fis_gRI74[] = { 1, 3, 8 };
int fis_gRI75[] = { 2, 1, 8 };
int fis_gRI76[] = { 2, 2, 8 };
int fis_gRI77[] = { 2, 3, 8 };
int fis_gRI78[] = { 3, 1, 8 };
int fis_gRI79[] = { 3, 2, 8 };
int fis_gRI80[] = { 3, 3, 8 };
int fis_gRI81[] = { 1, 1, 9 };
int fis_gRI82[] = { 1, 2, 9 };
int fis_gRI83[] = { 1, 3, 9 };
int fis_gRI84[] = { 2, 1, 9 };
int fis_gRI85[] = { 2, 2, 9 };
int fis_gRI86[] = { 2, 3, 9 };
int fis_gRI87[] = { 3, 1, 9 };
int fis_gRI88[] = { 3, 2, 9 };
int fis_gRI89[] = { 3, 3, 9 };
int fis_gRI90[] = { 1, 1, 10 };
int fis_gRI91[] = { 1, 2, 10 };
int fis_gRI92[] = { 1, 3, 10 };
int fis_gRI93[] = { 2, 1, 10 };
int fis_gRI94[] = { 2, 2, 10 };
int fis_gRI95[] = { 2, 3, 10 };
int fis_gRI96[] = { 3, 1, 10 };
int fis_gRI97[] = { 3, 2, 10 };
int fis_gRI98[] = { 3, 3, 10 };
int fis_gRI99[] = { 1, 1, 11 };
int fis_gRI100[] = { 1, 2, 11 };
int fis_gRI101[] = { 1, 3, 11 };
int fis_gRI102[] = { 2, 1, 11 };
int fis_gRI103[] = { 2, 2, 11 };
int fis_gRI104[] = { 2, 3, 11 };
int fis_gRI105[] = { 3, 1, 11 };
int fis_gRI106[] = { 3, 2, 11 };
int fis_gRI107[] = { 3, 3, 11 };
int fis_gRI108[] = { 1, 1, 12 };
int fis_gRI109[] = { 1, 2, 12 };
int fis_gRI110[] = { 1, 3, 12 };
int fis_gRI111[] = { 2, 1, 12 };
int fis_gRI112[] = { 2, 2, 12 };
int fis_gRI113[] = { 2, 3, 12 };
int fis_gRI114[] = { 3, 1, 12 };
int fis_gRI115[] = { 3, 2, 12 };
int fis_gRI116[] = { 3, 3, 12 };
int fis_gRI117[] = { 1, 1, 13 };
int fis_gRI118[] = { 1, 2, 13 };
int fis_gRI119[] = { 1, 3, 13 };
int fis_gRI120[] = { 2, 1, 13 };
int fis_gRI121[] = { 2, 2, 13 };
int fis_gRI122[] = { 2, 3, 13 };
int fis_gRI123[] = { 3, 1, 13 };
int fis_gRI124[] = { 3, 2, 13 };
int fis_gRI125[] = { 3, 3, 13 };
int fis_gRI126[] = { 1, 1, 14 };
int fis_gRI127[] = { 1, 2, 14 };
int fis_gRI128[] = { 1, 3, 14 };
int fis_gRI129[] = { 2, 1, 14 };
int fis_gRI130[] = { 2, 2, 14 };
int fis_gRI131[] = { 2, 3, 14 };
int fis_gRI132[] = { 3, 1, 14 };
int fis_gRI133[] = { 3, 2, 14 };
int fis_gRI134[] = { 3, 3, 14 };
int* fis_gRI[] = { fis_gRI0, fis_gRI1, fis_gRI2, fis_gRI3, fis_gRI4, fis_gRI5, fis_gRI6, fis_gRI7, fis_gRI8, fis_gRI9, fis_gRI10, fis_gRI11, fis_gRI12, fis_gRI13, fis_gRI14, fis_gRI15, fis_gRI16, fis_gRI17, fis_gRI18, fis_gRI19, fis_gRI20, fis_gRI21, fis_gRI22, fis_gRI23, fis_gRI24, fis_gRI25, fis_gRI26, fis_gRI27, fis_gRI28, fis_gRI29, fis_gRI30, fis_gRI31, fis_gRI32, fis_gRI33, fis_gRI34, fis_gRI35, fis_gRI36, fis_gRI37, fis_gRI38, fis_gRI39, fis_gRI40, fis_gRI41, fis_gRI42, fis_gRI43, fis_gRI44, fis_gRI45, fis_gRI46, fis_gRI47, fis_gRI48, fis_gRI49, fis_gRI50, fis_gRI51, fis_gRI52, fis_gRI53, fis_gRI54, fis_gRI55, fis_gRI56, fis_gRI57, fis_gRI58, fis_gRI59, fis_gRI60, fis_gRI61, fis_gRI62, fis_gRI63, fis_gRI64, fis_gRI65, fis_gRI66, fis_gRI67, fis_gRI68, fis_gRI69, fis_gRI70, fis_gRI71, fis_gRI72, fis_gRI73, fis_gRI74, fis_gRI75, fis_gRI76, fis_gRI77, fis_gRI78, fis_gRI79, fis_gRI80, fis_gRI81, fis_gRI82, fis_gRI83, fis_gRI84, fis_gRI85, fis_gRI86, fis_gRI87, fis_gRI88, fis_gRI89, fis_gRI90, fis_gRI91, fis_gRI92, fis_gRI93, fis_gRI94, fis_gRI95, fis_gRI96, fis_gRI97, fis_gRI98, fis_gRI99, fis_gRI100, fis_gRI101, fis_gRI102, fis_gRI103, fis_gRI104, fis_gRI105, fis_gRI106, fis_gRI107, fis_gRI108, fis_gRI109, fis_gRI110, fis_gRI111, fis_gRI112, fis_gRI113, fis_gRI114, fis_gRI115, fis_gRI116, fis_gRI117, fis_gRI118, fis_gRI119, fis_gRI120, fis_gRI121, fis_gRI122, fis_gRI123, fis_gRI124, fis_gRI125, fis_gRI126, fis_gRI127, fis_gRI128, fis_gRI129, fis_gRI130, fis_gRI131, fis_gRI132, fis_gRI133, fis_gRI134 };

// Rule Outputs
int fis_gRO0[] = { 3 };
int fis_gRO1[] = { 2 };
int fis_gRO2[] = { 1 };
int fis_gRO3[] = { 3 };
int fis_gRO4[] = { 3 };
int fis_gRO5[] = { 1 };
int fis_gRO6[] = { 4 };
int fis_gRO7[] = { 3 };
int fis_gRO8[] = { 1 };
int fis_gRO9[] = { 6 };
int fis_gRO10[] = { 5 };
int fis_gRO11[] = { 1 };
int fis_gRO12[] = { 6 };
int fis_gRO13[] = { 6 };
int fis_gRO14[] = { 1 };
int fis_gRO15[] = { 7 };
int fis_gRO16[] = { 6 };
int fis_gRO17[] = { 1 };
int fis_gRO18[] = { 7 };
int fis_gRO19[] = { 6 };
int fis_gRO20[] = { 1 };
int fis_gRO21[] = { 8 };
int fis_gRO22[] = { 7 };
int fis_gRO23[] = { 1 };
int fis_gRO24[] = { 8 };
int fis_gRO25[] = { 7 };
int fis_gRO26[] = { 1 };
int fis_gRO27[] = { 8 };
int fis_gRO28[] = { 7 };
int fis_gRO29[] = { 1 };
int fis_gRO30[] = { 9 };
int fis_gRO31[] = { 8 };
int fis_gRO32[] = { 1 };
int fis_gRO33[] = { 10 };
int fis_gRO34[] = { 9 };
int fis_gRO35[] = { 1 };
int fis_gRO36[] = { 11 };
int fis_gRO37[] = { 9 };
int fis_gRO38[] = { 1 };
int fis_gRO39[] = { 12 };
int fis_gRO40[] = { 11 };
int fis_gRO41[] = { 1 };
int fis_gRO42[] = { 14 };
int fis_gRO43[] = { 12 };
int fis_gRO44[] = { 1 };
int fis_gRO45[] = { 12 };
int fis_gRO46[] = { 14 };
int fis_gRO47[] = { 1 };
int fis_gRO48[] = { 13 };
int fis_gRO49[] = { 12 };
int fis_gRO50[] = { 1 };
int fis_gRO51[] = { 14 };
int fis_gRO52[] = { 13 };
int fis_gRO53[] = { 1 };
int fis_gRO54[] = { 12 };
int fis_gRO55[] = { 10 };
int fis_gRO56[] = { 1 };
int fis_gRO57[] = { 13 };
int fis_gRO58[] = { 12 };
int fis_gRO59[] = { 1 };
int fis_gRO60[] = { 14 };
int fis_gRO61[] = { 13 };
int fis_gRO62[] = { 1 };
int fis_gRO63[] = { 12 };
int fis_gRO64[] = { 11 };
int fis_gRO65[] = { 1 };
int fis_gRO66[] = { 14 };
int fis_gRO67[] = { 12 };
int fis_gRO68[] = { 1 };
int fis_gRO69[] = { 14 };
int fis_gRO70[] = { 13 };
int fis_gRO71[] = { 1 };
int fis_gRO72[] = { 13 };
int fis_gRO73[] = { 11 };
int fis_gRO74[] = { 1 };
int fis_gRO75[] = { 14 };
int fis_gRO76[] = { 13 };
int fis_gRO77[] = { 1 };
int fis_gRO78[] = { 14 };
int fis_gRO79[] = { 14 };
int fis_gRO80[] = { 1 };
int fis_gRO81[] = { 12 };
int fis_gRO82[] = { 10 };
int fis_gRO83[] = { 1 };
int fis_gRO84[] = { 13 };
int fis_gRO85[] = { 12 };
int fis_gRO86[] = { 1 };
int fis_gRO87[] = { 14 };
int fis_gRO88[] = { 13 };
int fis_gRO89[] = { 1 };
int fis_gRO90[] = { 11 };
int fis_gRO91[] = { 10 };
int fis_gRO92[] = { 1 };
int fis_gRO93[] = { 13 };
int fis_gRO94[] = { 11 };
int fis_gRO95[] = { 1 };
int fis_gRO96[] = { 14 };
int fis_gRO97[] = { 12 };
int fis_gRO98[] = { 1 };
int fis_gRO99[] = { 11 };
int fis_gRO100[] = { 10 };
int fis_gRO101[] = { 1 };
int fis_gRO102[] = { 13 };
int fis_gRO103[] = { 11 };
int fis_gRO104[] = { 1 };
int fis_gRO105[] = { 14 };
int fis_gRO106[] = { 13 };
int fis_gRO107[] = { 1 };
int fis_gRO108[] = { 9 };
int fis_gRO109[] = { 8 };
int fis_gRO110[] = { 1 };
int fis_gRO111[] = { 11 };
int fis_gRO112[] = { 9 };
int fis_gRO113[] = { 1 };
int fis_gRO114[] = { 12 };
int fis_gRO115[] = { 10 };
int fis_gRO116[] = { 1 };
int fis_gRO117[] = { 9 };
int fis_gRO118[] = { 8 };
int fis_gRO119[] = { 1 };
int fis_gRO120[] = { 10 };
int fis_gRO121[] = { 9 };
int fis_gRO122[] = { 1 };
int fis_gRO123[] = { 11 };
int fis_gRO124[] = { 9 };
int fis_gRO125[] = { 1 };
int fis_gRO126[] = { 7 };
int fis_gRO127[] = { 6 };
int fis_gRO128[] = { 1 };
int fis_gRO129[] = { 7 };
int fis_gRO130[] = { 6 };
int fis_gRO131[] = { 1 };
int fis_gRO132[] = { 8 };
int fis_gRO133[] = { 7 };
int fis_gRO134[] = { 1 };
int* fis_gRO[] = { fis_gRO0, fis_gRO1, fis_gRO2, fis_gRO3, fis_gRO4, fis_gRO5, fis_gRO6, fis_gRO7, fis_gRO8, fis_gRO9, fis_gRO10, fis_gRO11, fis_gRO12, fis_gRO13, fis_gRO14, fis_gRO15, fis_gRO16, fis_gRO17, fis_gRO18, fis_gRO19, fis_gRO20, fis_gRO21, fis_gRO22, fis_gRO23, fis_gRO24, fis_gRO25, fis_gRO26, fis_gRO27, fis_gRO28, fis_gRO29, fis_gRO30, fis_gRO31, fis_gRO32, fis_gRO33, fis_gRO34, fis_gRO35, fis_gRO36, fis_gRO37, fis_gRO38, fis_gRO39, fis_gRO40, fis_gRO41, fis_gRO42, fis_gRO43, fis_gRO44, fis_gRO45, fis_gRO46, fis_gRO47, fis_gRO48, fis_gRO49, fis_gRO50, fis_gRO51, fis_gRO52, fis_gRO53, fis_gRO54, fis_gRO55, fis_gRO56, fis_gRO57, fis_gRO58, fis_gRO59, fis_gRO60, fis_gRO61, fis_gRO62, fis_gRO63, fis_gRO64, fis_gRO65, fis_gRO66, fis_gRO67, fis_gRO68, fis_gRO69, fis_gRO70, fis_gRO71, fis_gRO72, fis_gRO73, fis_gRO74, fis_gRO75, fis_gRO76, fis_gRO77, fis_gRO78, fis_gRO79, fis_gRO80, fis_gRO81, fis_gRO82, fis_gRO83, fis_gRO84, fis_gRO85, fis_gRO86, fis_gRO87, fis_gRO88, fis_gRO89, fis_gRO90, fis_gRO91, fis_gRO92, fis_gRO93, fis_gRO94, fis_gRO95, fis_gRO96, fis_gRO97, fis_gRO98, fis_gRO99, fis_gRO100, fis_gRO101, fis_gRO102, fis_gRO103, fis_gRO104, fis_gRO105, fis_gRO106, fis_gRO107, fis_gRO108, fis_gRO109, fis_gRO110, fis_gRO111, fis_gRO112, fis_gRO113, fis_gRO114, fis_gRO115, fis_gRO116, fis_gRO117, fis_gRO118, fis_gRO119, fis_gRO120, fis_gRO121, fis_gRO122, fis_gRO123, fis_gRO124, fis_gRO125, fis_gRO126, fis_gRO127, fis_gRO128, fis_gRO129, fis_gRO130, fis_gRO131, fis_gRO132, fis_gRO133, fis_gRO134 };

// Input range Min
FIS_TYPE fis_gIMin[] = { 0, 0, 0 };

// Input range Max
FIS_TYPE fis_gIMax[] = { 35, 100, 16 };

// Output range Min
FIS_TYPE fis_gOMin[] = { 0 };

// Output range Max
FIS_TYPE fis_gOMax[] = { 250 };

//***********************************************************************
// Data dependent support functions for Fuzzy Inference System           
//***********************************************************************
FIS_TYPE fis_MF_out(FIS_TYPE** fuzzyRuleSet, FIS_TYPE x, int o)
{
    FIS_TYPE mfOut;
    int r;

    for (r = 0; r < fis_gcR; ++r)
    {
        int index = fis_gRO[r][o];
        if (index > 0)
        {
            index = index - 1;
            mfOut = (fis_gMF[fis_gMFO[o][index]])(x, fis_gMFOCoeff[o][index]);
        }
        else if (index < 0)
        {
            index = -index - 1;
            mfOut = 1 - (fis_gMF[fis_gMFO[o][index]])(x, fis_gMFOCoeff[o][index]);
        }
        else
        {
            mfOut = 0;
        }

        fuzzyRuleSet[0][r] = fis_min(mfOut, fuzzyRuleSet[1][r]);
    }
    return fis_array_operation(fuzzyRuleSet[0], fis_gcR, fis_max);
}

FIS_TYPE fis_defuzz_centroid(FIS_TYPE** fuzzyRuleSet, int o)
{
    FIS_TYPE step = (fis_gOMax[o] - fis_gOMin[o]) / (FIS_RESOLUSION - 1);
    FIS_TYPE area = 0;
    FIS_TYPE momentum = 0;
    FIS_TYPE dist, slice;
    int i;

    // calculate the area under the curve formed by the MF outputs
    for (i = 0; i < FIS_RESOLUSION; ++i){
        dist = fis_gOMin[o] + (step * i);
        slice = step * fis_MF_out(fuzzyRuleSet, dist, o);
        area += slice;
        momentum += slice*dist;
    }

    return ((area == 0) ? ((fis_gOMax[o] + fis_gOMin[o]) / 2) : (momentum / area));
}

//***********************************************************************
// Fuzzy Inference System                                                
//***********************************************************************
void fis_evaluate()
{
    FIS_TYPE fuzzyInput0[] = { 0, 0, 0 };
    FIS_TYPE fuzzyInput1[] = { 0, 0, 0 };
    FIS_TYPE fuzzyInput2[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    FIS_TYPE* fuzzyInput[fis_gcI] = { fuzzyInput0, fuzzyInput1, fuzzyInput2, };
    FIS_TYPE fuzzyOutput0[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    FIS_TYPE* fuzzyOutput[fis_gcO] = { fuzzyOutput0, };
    FIS_TYPE fuzzyRules[fis_gcR] = { 0 };
    FIS_TYPE fuzzyFires[fis_gcR] = { 0 };
    FIS_TYPE* fuzzyRuleSet[] = { fuzzyRules, fuzzyFires };
    FIS_TYPE sW = 0;

    // Transforming input to fuzzy Input
    int i, j, r, o;
    for (i = 0; i < fis_gcI; ++i)
    {
        for (j = 0; j < fis_gIMFCount[i]; ++j)
        {
            fuzzyInput[i][j] =
                (fis_gMF[fis_gMFI[i][j]])(g_fisInput[i], fis_gMFICoeff[i][j]);
        }
    }

    int index = 0;
    for (r = 0; r < fis_gcR; ++r)
    {
        if (fis_gRType[r] == 1)
        {
            fuzzyFires[r] = FIS_MAX;
            for (i = 0; i < fis_gcI; ++i)
            {
                index = fis_gRI[r][i];
                if (index > 0)
                    fuzzyFires[r] = fis_min(fuzzyFires[r], fuzzyInput[i][index - 1]);
                else if (index < 0)
                    fuzzyFires[r] = fis_min(fuzzyFires[r], 1 - fuzzyInput[i][-index - 1]);
                else
                    fuzzyFires[r] = fis_min(fuzzyFires[r], 1);
            }
        }
        else
        {
            fuzzyFires[r] = FIS_MIN;
            for (i = 0; i < fis_gcI; ++i)
            {
                index = fis_gRI[r][i];
                if (index > 0)
                    fuzzyFires[r] = fis_max(fuzzyFires[r], fuzzyInput[i][index - 1]);
                else if (index < 0)
                    fuzzyFires[r] = fis_max(fuzzyFires[r], 1 - fuzzyInput[i][-index - 1]);
                else
                    fuzzyFires[r] = fis_max(fuzzyFires[r], 0);
            }
        }

        fuzzyFires[r] = fis_gRWeight[r] * fuzzyFires[r];
        sW += fuzzyFires[r];
    }

    if (sW == 0)
    {
        for (o = 0; o < fis_gcO; ++o)
        {
            g_fisOutput[o] = ((fis_gOMax[o] + fis_gOMin[o]) / 2);
        }
    }
    else
    {
        for (o = 0; o < fis_gcO; ++o)
        {
            g_fisOutput[o] = fis_defuzz_centroid(fuzzyRuleSet, o);
        }
    }
}
