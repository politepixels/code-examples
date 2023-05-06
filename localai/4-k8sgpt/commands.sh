#!/usr/bin/env bash

helm repo add k8sgpt https://charts.k8sgpt.ai/
helm install k8sgpt-operator k8sgpt/k8sgpt-operator

kubectl create secret generic k8sgpt-sample-secret --from-literal=openai-api-key=$OPENAI_TOKEN -n default

kubectl run test --image nginx/test