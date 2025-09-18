param (
    [string]$logFileName
)

# Define the path to the Segger RTT logger executable
$programPath = "C:\Program Files\SEGGER\JLink_V812c\JLinkRTTLogger.exe"

# Define the input arguments for the Segger RTT logger
$arguments = @(
    "-Device", "CORTEX-M33",
    "-If", "SWD",
    "-Speed", "4000",
    "-RTTChannel", "5",
    "-LogToFile", $logFileName,
    "-RTTAddr", "0x020000a00"
)

# Optional: Add a message to indicate the Segger RTT logger has started with the input arguments
Write-Output "The Segger RTT logger has been started with the following arguments: $arguments"

# Start the Segger RTT logger with the input arguments
Start-Process -FilePath $programPath -ArgumentList $arguments -NoNewWindow -Wait -PassThru