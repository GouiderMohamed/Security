#include <WiFi.h>
#include <HTTPClient.h>

// Configuration Réseau
const char* ssid = "HG";
const char* password = "12345678";

// URL pointant vers votre serveur fonctionnel (Port 8080)
const char* serverUrl = "http://192.168.1.36:8082/"; 

const int TOTAL_MOTS = 3;

// Vos blocs clairs (HEX) exacts issus directement de vos logs serveur !
String mots_clairs[TOTAL_MOTS] = {
  "0072008582483DF4F4E59486E6B51035", // Correspond à la trame 1
  "9E2824EA8D44E60D08EFFE007C7FA0BA", // Correspond à la trame 2
  "FEB1104491183E8B6F09D761A8BF211F"  // Correspond à la trame 3
};

// Les blocs chiffrés envoyés par l'ESP32
String trames_chiffrees[TOTAL_MOTS] = {
  "60A90FBE6C6F2B8D8F492E38DFE1097F", 
  "885C77A5317EFE1B3DC1A8E2106A0B32", 
  "0F1EBB242EA5A351EE46B6B8BE945F33"  
};

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.print("[ESP32] Connexion Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n[ESP32] Connecté. Démarrage du protocole Défi/Réponse.");
  randomSeed(analogRead(0)); 
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");

    // Sélection aléatoire d'un des trois défis cryptographiques
    int index = random(0, TOTAL_MOTS);
    String mot_attendu = mots_clairs[index];
    String cipher_a_envoyer = trames_chiffrees[index];

    Serial.println("\n--------------------------------------------------");
    Serial.println("[ESP32] Transmission du bloc chiffré : " + cipher_a_envoyer);

    // Envoi de la trame au format JSON
    String jsonPayload = "{\"ciphertext\":\"" + cipher_a_envoyer + "\"}";
    int httpResponseCode = http.POST(jsonPayload);

    if (httpResponseCode == 200) {
      String responseBody = http.getString();
      
      // Extraction du champ "decrypted_word" dans la réponse JSON
      int idx = responseBody.indexOf("decrypted_word\":\"");
      if (idx != -1) {
        int finIdx = responseBody.indexOf("\"", idx + 17);
        String mot_recu = responseBody.substring(idx + 17, finIdx);
        
        Serial.println("[ESP32] Réponse brute du serveur : '" + mot_recu + "'");

        // Vérification de la réussite du défi
        if (mot_recu == mot_attendu) {
          Serial.println("[ESP32] RÉSULTAT : GAGNÉ ! Le serveur a déchiffré correctement avec votre code AES-128.");
        } else {
          Serial.println("[ESP32] RÉSULTAT : ÉCHEC ! Désalignement de l'algorithme.");
        }
      }
    } else {
      Serial.print("[ESP32] Erreur HTTP détectée : ");
      Serial.println(httpResponseCode);
    }
    http.end();
  }
  
  delay(6000); // Répétition de la boucle toutes les 6 secondes
}