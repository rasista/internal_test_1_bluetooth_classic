pipeline {
  agent
  {
      // everest docker image 
   label "nomad-sonarscan"
  }
  environment {
    WORKSPACE_PATH="${WORKSPACE}"
    SIM_PATH = "${WORKSPACE}/ut_frameworks/simpy_framework"
    BUILD_PATH = "${WORKSPACE}/build_si353_gcc"
  }
  triggers 
  {
        // Scheduled to run past midnight IST for every 3 days once
    cron('30 2 */3 * *')
  }
  stages {
    stage('Build Binary') {
      steps {
        script {
          properties([
                parameters([
                    string(
                        defaultValue: 'master',
                        name: 'branch_name', 
                        trim: true
                    )
                ])
            ])
          dir('${WORKSPACE_PATH}'){
            sh '''
              sh configure.sh
              cd "${WORKSPACE}/build_si353_gcc"
                 
                  
              make clean
              build-wrapper-linux-x86-64 --out-dir ./../../../build make
            '''
          }
        }
      }
    }
    stage('sonar-publish'){
      environment {
          GIT_HASH = GIT_COMMIT.take(10)
          SONAR = tool name: 'sonarscanner', type: 'hudson.plugins.sonar.SonarRunnerInstallation'
          SONAR_TOKEN = credentials('sonar_token')
      }
      steps{
        script{
          
          withSonarQubeEnv(installationName: 'SILABS_SONARQUBE_SERVER') {
              script {
                  sh '''
                      /usr/local/bin/sonar-scanner -X -Dsonar.projectVersion=${GIT_HASH} -Dsonar.cfamily.compile-commands=build/compile_commands.json  -Dsonar.branch.name=${branch_name}
                  '''                  
                  }
              }
              sleep time: 30, unit: 'SECONDS'
        }
      }
      post {
          success {
              script {
                  withCredentials([string(credentialsId:'sonar_token',variable:'SONAR_TOKEN')])
                  // Get SonarQube analysis results
                  {
                      def sonarQualityGateStatus = sh(returnStdout: true, script: """
                          curl -u $SONAR_TOKEN: \'https://sonarqube.silabs.net/api/qualitygates/project_status?projectKey=REDPINE_series_3_btdm_prototypes_AY4N4DAqLiRBxguCNZ7f&branch=${branch_name}\'
                      """)
                      def projectStatus = readJSON(text: sonarQualityGateStatus).projectStatus.status
                      def sonarProjectUrl = "https://sonarqube.silabs.net/dashboard?branch=${branch_name}&id=REDPINE_series_3_btdm_prototypes_AY4N4DAqLiRBxguCNZ7f"
                      println projectStatus
                      println sonarProjectUrl
                      def sonarIssuesJson = sh(returnStdout: true, script: """
                          curl -u $SONAR_TOKEN: \'https://sonarqube.silabs.net/api/measures/component?component=REDPINE_series_3_btdm_prototypes_AY4N4DAqLiRBxguCNZ7f&branch=${branch_name}&metricKeys=bugs,new_bugs,vulnerabilities,new_vulnerabilities,security_hotspots,new_security_hotspots,code_smells,new_code_smells,coverage,new_coverage,duplicated_lines,duplicated_lines_density\'
                      """)
                      println sonarIssuesJson
                      def json = readJSON(text: sonarIssuesJson)
                      def metrics = json.component.measures
                      metrics.each {
                          def metricName = it.metric
                          def metricValue = it.value ?: it.period.value
                          env."${metricName}" = metricValue ?: 0
                      }
                      // Print to Jenkins console logs
                      println env.bugs
                      emailext (
                          to: 'DL.DevOps.BuildAndRelease.Hyd@silabs.com,DL.ayghosh.org',
                          subject: "SonarQube Analysis Results for series_3_btdm_prototypes on branch ${BRANCH_NAME}",
                          body: """
                              <p>Dear recipients,</p>
                              <p>Please find the result of series_3_btdm_prototypes sonar scanning below:</p>
                              <p><strong>Branch: ${BRANCH_NAME}</strong></p>
                              <p><strong>Sonar Project Status: ${projectStatus}</strong></p>
                              <p>Sonar Project URL: <a href="${sonarProjectUrl}">${sonarProjectUrl}</a></p>
                              <table class="tg" style="border-style: hidden;">
                              <thead>
                              <tr>
                              <th class="tg-0lax"><strong>Metric</strong></th>
                              <th class="tg-0lax">Results</th>
                              </tr>
                              </thead>
                              <tbody>
                              <tr>
                              <td class="tg-de2y"><strong>Bugs</strong></td>
                              <td class="tg-de2y" style="text-align: right;">${env.bugs}</td>
                              </tr>
                              <tr>
                              <td class="tg-de2y"><strong>New bugs</strong></td>
                              <td class="tg-de2y" style="text-align: right;">${env.new_bugs ?: 0}</td>
                              </tr>
                              <tr>
                              <td class="tg-de2y"><strong>Vulnerabilities</strong></td>
                              <td class="tg-de2y" style="text-align: right;">${env.vulnerabilities}</td>
                              </tr>
                              <tr>
                              <td class="tg-de2y"><strong>New vulnerabilities</strong></td>
                              <td class="tg-de2y" style="text-align: right;">${env.new_vulnerabilities ?: 0}</td>
                              </tr>
                              <tr>
                              <td class="tg-de2y"><strong>Security hotspots</strong></td>
                              <td class="tg-de2y" style="text-align: right;">${env.security_hotspots}</td>
                              </tr>
                              <tr>
                              <td class="tg-de2y"><strong>New security hotspots</strong></td>
                              <td class="tg-de2y" style="text-align: right;">${env.new_security_hotspots ?: 0}</td>
                              </tr>
                              <tr>
                              <td class="tg-de2y"><strong>Code smells</strong></td>
                              <td class="tg-de2y" style="text-align: right;">${env.code_smells}</td>
                              </tr>
                              <tr>
                              <td class="tg-de2y"><strong>New code smells</strong></td>
                              <td class="tg-de2y" style="text-align: right;">${env.new_code_smells ?: 0}</td>
                              </tr>
                              <tr>
                              <td class="tg-de2y"><strong>Coverage</strong></td>
                              <td class="tg-de2y" style="text-align: right;">${env.coverage}</td>
                              </tr>
                              <tr>
                              <td class="tg-de2y"><strong>New coverage</strong></td>
                              <td class="tg-de2y" style="text-align: right;">${env.new_coverage ?: 0}</td>
                              </tr>
                              <tr>
                              <td class="tg-de2y"><strong>Duplicated lines</strong></td>
                              <td class="tg-de2y" style="text-align: right;">${env.duplicated_lines}</td>
                              </tr>
                              <tr>
                              <td class="tg-de2y"><strong>Duplicated lines density</strong></td>
                              <td class="tg-de2y" style="text-align: right;">${env.duplicated_lines_density}</td>
                              </tr>
                              </tbody>
                              </table>
                              <div>&nbsp;</div>
                              <div>Best regards,</div>
                              <div>The Jenkins Server</div>
                          """)
                  }
              }
          }
      }
    }
  }
}