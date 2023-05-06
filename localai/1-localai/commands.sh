#!/usr/bin/env bash

git clone https://github.com/go-skynet/LocalAI

cd LocalAI

wget https://gpt4all.io/models/ggml-gpt4all-j-v1.3-groovy.bin -O models/ggml-gpt4all-j-v1.3-groovy
cp prompt-templates/ggml-gpt4all-j.tmpl models/ggml-gpt4all-j-v1.3-groovy.tmpl

cat <<EOF > .env
THREADS=6
CONTEXT_SIZE=512
MODELS_PATH=/models
# DEBUG=true
# BUILD_TYPE=generic
EOF

docker compose up -d --build

curl http://localhost:8080/v1/models

curl http://localhost:8080/v1/chat/completions -H "Content-Type: application/json" -d '{
     "model": "ggml-gpt4all-j-v1.3-groovy",
     "messages": [{"role": "user", "content": "What is a cow?"}],
     "temperature": 0.1
   }'

curl http://localhost:8080/v1/completions -H "Content-Type: application/json" -d '{
     "model": "ggml-gpt4all-j-v1.3-groovy",
     "prompt": "What is a cow?",
     "temperature": 0.1
   }'

curl http://localhost:8080/v1/completions -H "Content-Type: application/json" -d '{
     "model": "ggml-gpt4all-j-v1.3-groovy",
     "prompt": "What is Kubernetes?",
     "temperature": 0.1
   }'
