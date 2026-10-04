@echo off
cls
echo ====================================================================
echo    LANCEMENT AUTOMATIQUE DU SCENARIO DOCKER (MAN-IN-THE-MIDDLE)
echo ====================================================================
echo.

:: 1. Démarrage des conteneurs s'ils ne sont pas lancés
echo [1/5] Verification et demarrage de l'infrastructure Docker...
docker-compose up -d
timeout /t 2 > nul
echo.

:: 2. Exécution de l'Émetteur
echo [2/5] DEBUT DU SCENARIO : L'EMETTEUR entre en action...
docker exec -it emetteur python -c "with open('trame_saine.bin', 'wb') as f: f.write(bytearray([0xD2]))"
echo -- OK : L'emetteur a genere la trame saine (0xD2) dans 'trame_saine.bin'.
echo.
timeout /t 2 > nul

:: 3. Exécution de l'Attaquant
echo [3/5] INTERCEPTION : L'ATTAQUANT pirate la ligne...
docker exec -it attaquant python attaquant.py
echo -- OK : L'attaquant a altere la trame en (0xD3) dans 'trame_corrompue.bin'.
echo.
timeout /t 2 > nul

:: 4. Exécution du Récepteur (Compilation et Vérification CRC en C)
echo [4/5] RECEPTION ET CONTROLE : Le RECEPTEUR verifie l'integrite...
echo --------------------------------------------------------------------
docker exec -it recepteur sh -c "gcc Correction_Err/CRC.c -o verif_crc && ./verif_crc"
echo --------------------------------------------------------------------
echo.
timeout /t 2 > nul

:: 5. Clôture de la démo
echo [5/5] Fin du scenario. Nettoyage de l'environnement Docker...
docker-compose down
echo.
echo ====================================================================
echo    DEMONSTRATION TERMINEE AVEC SUCCES !
echo ====================================================================
pause