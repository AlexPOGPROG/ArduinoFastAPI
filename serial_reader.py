import serial
import json

# Configuration du port série
serial_port = 'COM5'  # Remplacer par le port série de votre Arduino
baud_rate = 115200  # Définir le débit en bauds

# Ouverture de la connexion série
ser = serial.Serial(serial_port, baud_rate, timeout=1)

def reconstruct_data(delta_data, sample=3):
    # Vérifier si la liste de données delta est vide
    if not delta_data:
        return []
    
    # Initialiser la liste des données reconstruites avec le premier point de données réel
    reconstructed = [delta_data[0]/100]
    
    # Itérer sur les données delta
    for i in range(1, len(delta_data)):
        if i % sample == 0:
            # À chaque `sample` pas, commencer un nouveau keyframe
            reconstructed.append(delta_data[i]/100)
        else:
            # Sinon, ajouter le delta à la dernière valeur de keyframe
            reconstructed.append(reconstructed[-1] + delta_data[i]/100)
    
    return reconstructed

def get_size_in_bytes(data):
    """ Calculer la taille des données en octets lorsqu'elles sont converties en chaîne JSON. """
    return len(json.dumps(data).encode('utf-8'))

def calculate_compression_rate(data):
    """ Calculer et afficher le taux de compression donné des données non compressées et compressées. """
    try:
        # Assurer que les données nécessaires sont présentes
        if 'PressionsBrutes' in data and 'PressionsCompresses' in data:
            size_brutes = get_size_in_bytes(data['PressionsBrutes'])
            size_compresses = get_size_in_bytes(data['PressionsCompresses'])
            
            # Éviter la division par zéro
            if size_compresses == 0:
                raise ValueError("La taille des données compressées est zéro, impossible de calculer le taux de compression.")
            
            compression_rate = size_brutes / size_compresses
            print(f"Taille des données non compressées: {size_brutes} octets")
            print(f"Taille des données compressées: {size_compresses} octets")
            print(f"Taux de compression: {compression_rate:.2f}")
        else:
            print("L'objet des données ne contient pas les clés requises.")
    except Exception as e:
        print(f"Une erreur s'est produite: {str(e)}")

def read_sensor_data():
    try:
        line = ser.readline()
        decoded_line = line.decode('utf-8').strip()
        if decoded_line:
            data = json.loads(decoded_line)
            if "PressionsCompresses" in data:
                data["PressionsReconstruites"] = reconstruct_data(data["PressionsCompresses"])
            return data
        return {"error": "Aucune ligne reçue"}
    except Exception as e:
        return {"error": str(e), "message": "Une erreur inattendue s'est produite."}

# Tester la lecture série
if __name__ == "__main__":
    while True:
        sensor_data = read_sensor_data()
        if sensor_data and "error" not in sensor_data:
            print(sensor_data)
        else:
            print(sensor_data.get("error", "Erreur inconnue"))
