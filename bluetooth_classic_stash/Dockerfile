FROM dkrhel001.silabs.com/toolchain/gsdk:24q2
SHELL ["/bin/bash", "-o", "pipefail", "-c"]

################################################# This section is maintained by DevOps team ###################################################
ENV DEBIAN_FRONTEND=noninteractive

ENV DEVOPS_DOCKERFILE="https://stash.silabs.com/projects/BTDM/repos/bluetooth_classic/Dockerfile"
ENV DEVOPS_JENKINS_UID=990
ENV DEVOPS_JENKINS_GID=990
ENV DEVOPS_JENKINS_HOME=/home/jenkins
ENV DEVOPS_AGENT_WORKDIR=${DEVOPS_JENKINS_HOME}/agent
ENV DEVOPS_JAVA_VERSION=17

ENV JAVA_HOME=/usr/lib/jvm/java-${DEVOPS_JAVA_VERSION}-amazon-corretto
ENV PATH="${JAVA_HOME}/bin:${PATH}"

# Set workDir
WORKDIR /tmp/build
COPY ./devops_dependencies.sh .

USER root

RUN  bash devops_dependencies.sh && \
    rm -rf /tmp/build

############################################## Intermediate step for local building ##########################################################
ARG RUN_ENV="cicd"
ARG LOCAL_USER="jenkins"
ARG LOCAL_USER_UID=${DEVOPS_JENKINS_UID}
ARG LOCAL_USER_GID=${DEVOPS_JENKINS_GID}

RUN if [[ "${RUN_ENV}" == "local" ]]; then \
    apt-get update && apt-get install -y --no-install-recommends sudo \
    && groupadd --gid "${LOCAL_USER_GID}" "${LOCAL_USER}" \
    && useradd -s /bin/bash --uid "${LOCAL_USER_UID}" --gid "${LOCAL_USER_GID}" -m "${LOCAL_USER}" \
    && echo "${LOCAL_USER} ALL=(ALL) NOPASSWD:ALL" > "/etc/sudoers.d/${LOCAL_USER}" \
    && chmod 0440 "/etc/sudoers.d/${LOCAL_USER}"; \
    fi
# Add the local bin directory to PATH
ENV PATH="/home/${LOCAL_USER}/.local/bin:${PATH}"

############################################### This section is maintained by Bluetooth Classic ################################################
RUN build_deps='cmake doxygen gnupg2 software-properties-common apt-transport-https ca-certificates   \
    gdb build-essential socat valgrind' \
    && apt-get update \
    && apt-get install -y --no-install-recommends ${build_deps}

USER ${LOCAL_USER}

RUN py_deps='simpy html2text jinja2 markupsafe pyxb pyyaml ruyaml numpy xmltodict pytest scipy \
    pytest-html junit-xml pyserial pyelftools>=0.30' && \
    python3 -m pip install --no-cache-dir --break-system-packages --upgrade certifi && \
    python3 -m pip install --break-system-packages ${py_deps}

######################################################### Cleanup ############################################################################
USER root
RUN apt-get clean && \
    rm -rf /var/lib/apt/lists/*

USER ${LOCAL_USER}
WORKDIR "/home/${LOCAL_USER}"
CMD [ "/bin/bash" ]