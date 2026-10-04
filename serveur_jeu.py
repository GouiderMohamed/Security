from http.server import BaseHTTPRequestHandler, HTTPServer
import json
import sys
import os

# Importation dynamique de votre fichier aes.py selon l'arborescence
sys.path.append(os.path.join(os.path.dirname(__file__), 'AES_DES', 'Version_py'))
try:
    import aes
except ImportError:
    import aes

# Clé officielle extraite de votre fichier main.py (16 octets)
SHARED_KEY = bytes.fromhex("2B7E151628AED2A6ABF7158809CF4F3C")

class CryptoGameServer(BaseHTTPRequestHandler):
    def do_POST(self):
        content_length = int(self.headers['Content-Length'])
        body = self.rfile.read(content_length).decode('utf-8')
        data = json.loads(body)
        
        # Réception de la chaîne hexadécimale envoyée par l'ESP32
        ciphertext_hex = data.get("ciphertext", "")
        print(f"\n[SERVEUR] Défi Ciphertext reçu : {ciphertext_hex}")
        
        try:
            ciphertext_bytes = bytes.fromhex(ciphertext_hex)
            
            # Déchiffrement réel avec votre implémentation de aes.py
            decrypted_bytes = aes.aes_decrypt(ciphertext_bytes, SHARED_KEY)
            
            # On transforme le résultat en chaîne HEX majuscule (comme dans votre main.py)
            resultat_hex = decrypted_bytes.hex().upper()
            print(f"[SERVEUR] Bloc déchiffré avec votre code (HEX) : '{resultat_hex}'")
            
            # Envoi de la réponse JSON à la carte
            self.send_response(200)
            self.send_header('Content-type', 'application/json')
            self.end_headers()
            
            # On renvoie la valeur sous forme HEX à l'ESP32
            response_json = {"status": "success", "decrypted_word": resultat_hex}
            self.wfile.write(json.dumps(response_json).encode('utf-8'))
            
        except Exception as e:
            print(f"[SERVEUR] Erreur de traitement : {e}")
            self.send_response(500)
            self.end_headers()

if __name__ == "__main__":
    server = HTTPServer(('0.0.0.0', 8082), CryptoGameServer)
    print("=== SERVEUR DÉFI AES-128 LANCÉ SUR LE PORT 8080 ===")
    server.serve_forever()