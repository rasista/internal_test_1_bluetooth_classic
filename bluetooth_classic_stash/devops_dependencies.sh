#!/bin/bash

# Create jenkins group and user
groupadd -g "${DEVOPS_JENKINS_GID:-990}" "jenkins"
useradd -c "Jenkins service account" -d "/home/jenkins" -u "${DEVOPS_JENKINS_UID:-990}" -g "${DEVOPS_JENKINS_GID:-990}" -m "jenkins"
apt-get update
apt-get install --no-install-recommends -y wget curl gnupg2 software-properties-common

# Add amazon coretto apt repository
wget -q https://apt.corretto.aws/corretto.key && gpg --dearmor -o /usr/share/keyrings/corretto-keyring.gpg corretto.key && \
echo "deb [signed-by=/usr/share/keyrings/corretto-keyring.gpg] https://apt.corretto.aws stable main" | tee /etc/apt/sources.list.d/corretto.list

# Add jfrog artifactory apt repository
wget -q https://releases.jfrog.io/artifactory/jfrog-gpg-public/jfrog_public_gpg.key && \
gpg --dearmor -o /usr/share/keyrings/jfrog-keyring.gpg jfrog_public_gpg.key && \
echo "deb [signed-by=/usr/share/keyrings/jfrog-keyring.gpg] https://releases.jfrog.io/artifactory/jfrog-debs xenial contrib" | tee /etc/apt/sources.list.d/jfrog.list

# Install apt packages
apt-get update
apt-get upgrade -y
apt-get install --no-install-recommends -y jq \
	bc \
	ca-certificates \
	docker.io \
	git \
	git-lfs \
	"java-${DEVOPS_JAVA_VERSION:-17}-amazon-corretto-jdk" \
	jfrog-cli-v2-jf \
	java-common \
	openssl \
	openssh-client \
	python3 \
	python3-pip \
	python3-venv \
	unzip \
	vim \
	xz-utils \
	zip

# Install ca-certificates
mkdir -p /tmp/certs/crt/
# SL ROOT (depreciated)
curl http://pkicorp.silabs.com/certdata/SL%20Root%20CA.crt >/tmp/certs/ceraus011-ca.der
curl http://pkicorp.silabs.com/certdata/SL%20Enterprise%20CA01.crt >/tmp/certs/ceraus012-ca.der
# VAULT CERT
curl --insecure https://vault.silabs.net/v1/pki/ca/pem >/tmp/certs/vault_ca_prod.pem
curl --insecure https://vault.dev.silabs.net/v1/pki/ca/pem >/tmp/certs/vault_ca_dev.pem

openssl x509 -inform der -in /tmp/certs/ceraus011-ca.der -out /tmp/certs/crt/ceraus011-ca.crt
openssl x509 -inform der -in /tmp/certs/ceraus012-ca.der -out /tmp/certs/crt/ceraus012-ca.crt
openssl x509 -inform pem -in /tmp/certs/vault_ca_prod.pem -out /tmp/certs/crt/vault_ca_prod.crt
openssl x509 -inform pem -in /tmp/certs/vault_ca_dev.pem -out /tmp/certs/crt/vault_ca_dev.crt

cat /tmp/certs/crt/*.crt >/usr/local/share/ca-certificates/ceraus.pem
cp -vrf /tmp/certs/crt/*.crt /usr/local/share/ca-certificates

# Update ca-certificates
update-ca-certificates

# Update java keytool cert-store
for cert in /tmp/certs/crt/*.crt; do
	"${JAVA_HOME}/bin/keytool" -import -noprompt -cacerts -storepass changeit -trustcacerts -alias "$(basename "${cert}")" -file "${cert}"
done
rm -rf /tmp/certs
