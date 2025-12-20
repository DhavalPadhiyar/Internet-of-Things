import cv2
import face_recognition
import os
import requests

#configure the esp ip and urland face folder path
ESP_IP = "10.189.212.184"
URL = f"http://{ESP_IP}/name"
faces_folder = "faces"
tolerance = 0.5  # Lower = stricter matching
use_cnn = False  # Set True for CNN model (slower but more accurate)
resize_factor = 0.25  # Resize for faster processing

# Load known faces safely
known_encodings = []
known_names = []

for person in os.listdir(faces_folder):
    person_path = os.path.join(faces_folder, person)
    if not os.path.isdir(person_path):
        continue

    print(f"Processing images for: {person}")
    for img_file in os.listdir(person_path):
        img_path = os.path.join(person_path, img_file)
        try:
            image = face_recognition.load_image_file(img_path)
            encodings = face_recognition.face_encodings(image)
            if len(encodings) == 0:
                print(f" No face found in {img_file}, skipping...")
                continue

            encoding = encodings[0]
            known_encodings.append(encoding)
            known_names.append(person)
        except Exception as e:
            print(f" Error processing {img_file}: {e}")

print("All images processed successfully!")
print(f"Known people: {set(known_names)}")

# setting up camera 
cap = cv2.VideoCapture(0)
last_name = ""

while True:
    ret, frame = cap.read()
    if not ret:
        print("Failed to grab frame from camera")
        break

    # Resize frame for faster processing
    small_frame = cv2.resize(frame, (0, 0), fx=resize_factor, fy=resize_factor)
    rgb_small = cv2.cvtColor(small_frame, cv2.COLOR_BGR2RGB)

    # Detect faces
    model_type = "cnn" if use_cnn else "hog"
    face_locations = face_recognition.face_locations(rgb_small, model=model_type)
    face_encodings = face_recognition.face_encodings(rgb_small, face_locations)

    # Scale back face locations to original frame size
    face_locations = [(top / resize_factor, right / resize_factor, bottom / resize_factor, left / resize_factor)
                      for (top, right, bottom, left) in face_locations]

    # Recognize faces
    for encoding, loc in zip(face_encodings, face_locations):
        matches = face_recognition.compare_faces(known_encodings, encoding, tolerance=tolerance)
        name = "Unknown"

        if True in matches:
            name = known_names[matches.index(True)]

        # Send name to ESP only if it changed
        if name != last_name:
            try:
                requests.get(URL, params={"person": name}, timeout=5)
                print(f"Sent to ESP: {name}")
            except requests.RequestException as e:
                print(f" Error sending to ESP: {e}")
            last_name = name

        # Draw rectangle and label
        top, right, bottom, left = [int(x) for x in loc]
        cv2.rectangle(frame, (left, top), (right, bottom), (0, 255, 0), 2)
        cv2.putText(frame, name, (left, top - 10),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.9, (0, 255, 0), 2)

    # Show the camera feed
    cv2.imshow("Face Recognition", frame)

    # Exit on ESC key
    if cv2.waitKey(1) == 27:
        break

cap.release()
cv2.destroyAllWindows()

