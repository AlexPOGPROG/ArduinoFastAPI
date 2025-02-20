#include <Arduino_LSM9DS1.h>
#include <Arduino_LPS22HB.h>
#define N_sample 3

int sample = 0;
int temps = 10000;
int freq = 20;
const int taille = 200;
int pressions[taille];
float pressionsRaw[taille];

int *ptr = pressions;
float *ptr2 = pressionsRaw;
//*ptr pointe vers le premier élément de pressions

void setup() {
  Serial.begin(115200);
  while (!Serial);

  if (!BARO.begin()) {
    Serial.println("Failed to initialize pressure sensor!");
    while (1);
  }
}

//envoyer C réinitialise la liste
void commandec(char receivedChar){
  if (receivedChar == 'C' || receivedChar == 'c'){
    for (int i = 0; i < taille; i++) {
    pressions[i] = 0;
    pressionsRaw[i] = 0;
    }
  Serial.println("Réinitiallisé");
    ptr = pressions;
    ptr2 = pressionsRaw;
  sample = 0;
  }
}


int sizeReduction(){
  int uncompressedData = 6;
  int deltaEncoding_Data = 2;
  int kefFrame_Data = 5;

  int uncompressedDatas = 6 * taille;
  int compressedDatas = 2 * (taille - 67) + 5 * 67;

  int sizeDifference = uncompressedDatas - compressedDatas;
  return(sizeDifference);
}

void loop() {
  if (Serial.available()){
    char receivedChar = Serial.read(); 
    Serial.println(receivedChar);
    commandec(receivedChar);
    //la fonction de réinitialisation est appelée avec le caractère rentré
  }
    //si l'adresse de l'élément parcouru soustrait à celle de premier élément est plus petite que la taille :
  if((ptr - pressions) < taille)
  {
      

    float pression = BARO.readPressure();
    //on récupère la valeur actuelle
    
    *ptr = pression*100;
    *ptr2= pression;
    //la valeur pointée par ptr devient la valeur actuelle
    
    //sample représente la distance du keyframe.
    //si sample est non nul (on est après le keyframe) et sample n'excède pas le nombre de valeurs à encoder(N_sample)
    if(sample != 0 && sample < N_sample)
    {
      //tant que i est plus petit que sample,on l'incrcémente et on soustrait la valeur actuelle de la pression aux valeurs précédentes jusqu'à la keyframe
      for (int i = 1; i <= sample; i++)
      {
        *ptr -= *(ptr-i);
      }
      //à la fin de la boucle, on rajoute 1 au sample pour exprimer qu'on vient de rajouter une valeur encodée
      sample++;
    }
    //si initialement sample était nul (cas lors du lancement) ou valait le nombre de valeurs à encoder (N_sample), on le remets à 1, et dans ces cas là la valeur pointée par ptr reste "pression"
    else
    {
      sample = 1;
    }


  // print the sensor value
  Serial.print(*ptr);
  Serial.println(BARO.readPressure());

  // print an empty line
  Serial.println();

// ptr prend l'adresse suivante dans le tableau
 ptr++;
 ptr2++;

  // wait 1 second to print again
  delay(50);

  }


  else {
  Serial.print("{");  // Début de l'objet JSON
  Serial.print("\"PressionsCompresses\":[");

  // Affichage de la liste des pressions compressées
  for (int i = 0; i < taille; i++) {
    Serial.print(pressions[i]);
    if (i < taille - 1) {
      Serial.print(",");
    }
  }
  Serial.print("],");

  Serial.print("\"PressionsBrutes\":[");

  // Affichage de la liste des pressions brutes
  for (int i = 0; i < taille; i++) {
    Serial.print(pressionsRaw[i]);
    if (i < taille - 1) {
      Serial.print(",");
    }
  }
  Serial.print("],");

  int sizeDiff = sizeReduction();

  Serial.print("\"sizeDifference\":[");
  Serial.print(sizeDiff);
  Serial.print("]}");

// Fin de l'objet JSON
  

  delay(10000);}  // Pause avant la prochaine série de données
}

  
