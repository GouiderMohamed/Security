import os

# L'attaquant intercepte la trame saine générée par l'émetteur
if os.path.exists("trame_saine.bin"):
    with open("trame_saine.bin", "rb") as f:
        data = bytearray(f.read())
    
    if len(data) > 0:
        # Simulation d'attaque : on inverse le premier bit du premier octet (XOR 0x01)
        print(f"[ATTAQUANT] Interception de la trame. Donnée originale : {hex(data[0])}")
        data[0] ^= 0x01 
        print(f"[ATTAQUANT] Injection d'erreur réussie. Nouvelle donnée : {hex(data[0])}")
        
        # Envoi de la trame corrompue au récepteur
        with open("trame_corrompue.bin", "wb") as f:
            f.write(data)
else:
    print("[ATTAQUANT] Erreur : Aucune trame saine interceptée.")