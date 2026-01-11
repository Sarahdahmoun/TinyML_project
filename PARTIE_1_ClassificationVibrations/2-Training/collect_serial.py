import serial, csv, time, os
from datetime import datetime

PORT = "COM5"  
# Vitesse de communication égale à celle du Serial.begin     
BAUD = 115200
#Classes associées au données enregistrées, à changer avant chaque enregistrement
LABEL = "dysfunction"        # off / normal / fault
DURATION = 120       # secondes
# Nom du dossier où seront stockés les fichiers CSV
OUT_DIR = "vibration_data"

# Création du dossier
os.makedirs(OUT_DIR, exist_ok=True)
# Création d'un nom de fichier avec la date et l'heur 
fname = f"{LABEL}_{datetime.now().strftime('%Y%m%d_%H%M%S')}.csv"
# Chemin complet du fichier
path = os.path.join(OUT_DIR, fname)

#Ouverture de la communication série avec l'Arduino 
ser = serial.Serial(PORT, BAUD, timeout=1)
time.sleep(2)

# Ouverture du fichier CSV en écriture 
with open(path, "w", newline="") as f:
    w = csv.writer(f)
    w.writerow(["label", "t_ms", "ax", "ay", "az"])
    #Heure de début
    start = time.time()
    #Boucle tourne durant DURATION (120) secondes
    while time.time() - start < DURATION:
	# Lecture d'une ligne envoyée par l'Arduino
        line = ser.readline().decode("utf-8", errors="ignore").strip()
	# On ignore tout ce qui ne commence pas par "ACC,"
        if not line.startswith("ACC,"):
            continue
	# On découpe la ligne selon les virgules
        parts = line.split(",")
	# Si la ligne n'a pas exactement 5 éléments, on l'ignore
        if len(parts) != 5:
            continue
	# On sépare les éléments sans ACC
        _, t_ms, ax, ay, az = parts
	
	# On sépare les éléments
        w.writerow([LABEL, t_ms, ax, ay, az])

# Fermeture de la connexion série
ser.close()
print("Saved:", path)
