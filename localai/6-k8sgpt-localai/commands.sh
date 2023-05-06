#!/usr/bin/env bash

curl http://localhost:8080/v1/completions -H "Content-Type: application/json" -d '{
     "model": "ggml-gpt4all-l13b-snoozy",
     "prompt": "out of memory",
     "temperature": 0.1
   }'