#!/usr/bin/env bash

docker compose down

wget https://gpt4all.io/models/ggml-gpt4all-l13b-snoozy.bin -O models/ggml-gpt4all-l13b-snoozy
cp prompt-templates/ggml-gpt4all-j.tmpl models/ggml-gpt4all-l13b-snoozy.tmpl

docker compose up -d --build

curl http://localhost:8080/v1/models

curl http://localhost:8080/v1/completions -H "Content-Type: application/json" -d '{
     "model": "ggml-gpt4all-l13b-snoozy",
     "prompt": "What is a cow?",
     "temperature": 0.1
   }'

curl http://localhost:8080/v1/completions -H "Content-Type: application/json" -d '{
     "model": "ggml-gpt4all-l13b-snoozy",
     "prompt": "What is Kubernetes?",
     "temperature": 0.1
   }'
