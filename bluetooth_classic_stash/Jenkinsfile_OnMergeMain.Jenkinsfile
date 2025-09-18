@Library(['common-ci-scripts']) _

pipeline {
  agent {
    label 'nomad-everest'
  }

  triggers {
    cron('30 0 * * 1')
  }

  options {
    parallelsAlwaysFailFast()
    skipDefaultCheckout(true)
  }

  parameters {
    string(name: 'BRANCH_NAME_STRING', defaultValue: '', description: 'Branch name to use for the build (leave empty to use default)')
  }

  environment {
    GSDK="${WORKSPACE}/gsdk"
    BTC="${WORKSPACE}/gsdk/protocol/bluetooth_classic"
    WORKSPACE_PATH="${BTC}"
    PYTHONPATH="${BTC}/ut_frameworks/utf:${BTC}/ut_frameworks/utf/utf_lib_btdm"
    HARNESS_DIR="${BTC}/ut_frameworks/utf/config/"
    SIM_PATH = "${BTC}/ut_frameworks/simpy_framework"
    BUILD_PATH = "${BTC}/bin"
    HARNESS_NAME="example_harness.yaml"
    UART_PORTS_PATH="/dev/pts"
    BRANCH_NAME = "${params.BRANCH_NAME_STRING ?: 'main'}"
    JENKINS_MERGE_ID = "${env.BUILD_NUMBER}"
  }
  stages {
    stage('checkout'){
      steps{
        script{
          Map checkoutStages = [
            "Clone - bluetooth_classic" : checkoutSCMStage(
              path:'bluetooth_classic',
              branches: [BRANCH_NAME],
              'https://stash.silabs.com/scm/btdm_audio/bluetooth_classic.git'
            ),
            "Clone - gsdk" : checkoutSCMStage(
              path:'gsdk',
              branches: ['main'],
              shallow: true,
              lfs: true,
              'https://stash-mirror.silabs.com/scm/gsdk/gsdk.git'
            ),
          ]
          parallel checkoutStages
        }
      }
    }
    stage('Patch GSDK') {
      steps {
        script {
          dir('gsdk'){
            sh '''
              cp -rf ../bluetooth_classic protocol/bluetooth_classic
            '''
          }
        }
      }
    } 
    stage('get sl_btdm_log version')
    {
      steps {
        script {
          dir('gsdk/protocol/bluetooth_classic/src/utils/debug/sl_btdm_logging'){
            sh '''
              git log -n 2
            '''
          }
        }
      }
    }
    stage('Signature Trust SDK') {
      steps {
        script {
            sh """
              slc signature trust --sdk ${GSDK}
            """
        }
      }
    }
    stage('Build Rail Library sixg300xilpwh74000fpga') {
        steps {
            script {
                dir('gsdk/platform/radio/rail_lib') {
                    sh '''
                      make clean ; make librail_sixg300xilpwh74000fpga_gcc_debug RAIL_LIB_DEV=1
                    '''
                }
            }
        }
    }
    stage("Build Bluetooth Classic SDK and lite controller sixg300xilpwh74000fpga for sequencer ") {
      steps {
          script {
              dir("${BTC}") {
                  sh """
                    sh build_system_librailsixg300_74k_jenkins.sh  -c 0 -r 0
                  """
              }
          }
      }
    }
    stage('Run and Generate Code Size analysis Report') {
      steps {
        script{
          dir("${BTC}") {
            sh """
              # Setup the virtual environment for running code size analysis
              python3 -m venv code_size_analysis_venv
              . code_size_analysis_venv/bin/activate
              python -m pip install --upgrade pip
              pip install 'code_size_analyzer_client-python>=1.0.0'
              cd "${BTC}/src/lc/llc"
              cmake --build ./build --target btc_riscvseq_code_size_analysis
            """
          }
        }
      }
    }
  }
  post {
    always {
      script{
        emailext (
          subject: "Build on ${env.JOB_NAME} is ${currentBuild.currentResult} ",
          body: " Build for ${env.JOB_NAME} is complete.\n\nCheck the build log here: ${env.BUILD_URL}",
          to: "DL.DevOps.BuildAndRelease.Hyd@silabs.com,DL.ayghosh.staff@silabs.com", // Change to your recipient email address
        )
        publishHTML([allowMissing: false, alwaysLinkToLastBuild: false, keepAll: true, reportDir: 'gsdk/protocol/bluetooth_classic/src/lc/llc/build/riscvseq_interface', reportFiles: 'btc_riscvseq_code_size_analysis.json', reportName: 'Code_Size_Analysis', reportTitles: ''])
        archiveArtifacts "gsdk/protocol/bluetooth_classic/src/lc/llc/build/riscvseq_interface/btc_riscvseq_code_size_analysis.json"
      }
    }
  }
}