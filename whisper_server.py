# whisper_server.py
import socket
import torch
import whisper
import numpy as np
import io

model = whisper.load_model("base")
HOST = '127.0.0.1'
PORT = 50007

print(f"Starting Whisper server on {HOST}:{PORT}")

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind((HOST, PORT))
    s.listen(1)
    conn, addr = s.accept()
    print('Connected by', addr)

    audio_data = bytearray()
    while True:
        data = conn.recv(3200)  # 1600 samples * 2 bytes/sample = 3200 bytes
        if not data:
            break
        audio_data.extend(data)

        if len(audio_data) >= 32000:  # ~1 second buffer (16000 samples)
            samples = np.frombuffer(audio_data[:32000], dtype=np.int16).astype(np.float32) / 32768.0
            audio_data = audio_data[32000:]

            audio = whisper.pad_or_trim(samples)
            mel = whisper.log_mel_spectrogram(audio).to(model.device)
            _, probs = model.detect_language(mel)
            options = whisper.DecodingOptions()
            result = whisper.decode(model, mel, options)

            print("🗣️ Transcription:", result.text)
