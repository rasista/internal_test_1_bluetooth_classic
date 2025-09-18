#!/usr/bin/env python
# Uncrustify Check Script v1.1
from __future__ import print_function
import fnmatch
import os
import sys
from subprocess import Popen, check_output, PIPE
import multiprocessing
import itertools

# Config options
uncrustify_cfg="uncrustify.cfg"
uncrustify_cfg_path=uncrustify_cfg
ignore_file=".uncrustify-ignore"
FILE_EXTS=[".c", ".h"]

# Allow the user to override what to use for uncrustifying
uncrustifyExe= "uncrustify"
if 'UNCRUSTIFY_EXE' in os.environ:
  uncrustifyExe= os.environ['UNCRUSTIFY_EXE']

def matchExt(filename):
  for ext in FILE_EXTS:
    if filename.endswith(ext):
      return True
  return False


def uncrustifyFile(file):
  gitShowCmd=["git", "show", ":"+file]
  uncrustifyCmd=[ uncrustifyExe,
                  '-c',
                  uncrustify_cfg_path,
                  '-l',
                  'C',
                  '-q',
                  '--check']
  p1 = Popen(gitShowCmd, shell=False, stdout=PIPE, stderr=PIPE)
  fileContent, err = p1.communicate()
  print("Standard error:", err.decode('utf-8'))
  print(err.decode("utf-8").replace("stdin", file).rstrip())
  if p1.returncode != 0:
    print(err.rstrip())
    return True
  p2 = Popen(uncrustifyCmd, shell=False, stdin=PIPE, stdout=PIPE, stderr=PIPE)
  out, err = p2.communicate(fileContent)
  print("Standard error:", err.decode('utf-8'))
  if p2.returncode != 0:
    print(err.decode("utf-8").replace("stdin", file).rstrip())
    return True

  return False


def runUncrustifyCheck(cfgPath=None, v1compat=True):
  """Run Uncrustify check on a repository

  Keyword arguments:
  cfgPath  -- Path to Uncrustify config file. If not given, the same directory as this script is used.
  v1compat -- Preserve compatibility with pre-commit hook v1.0 (hard sys.exit() instead of returning the error code to the parent).
  """
  print("Running Uncrustify check...")
  print("***********************************************************************")
  global uncrustify_cfg_path

  # If the cfgPath is None then use the directory of this script
  if cfgPath == None:
    cfgPath = os.path.normpath(os.path.dirname(os.path.abspath(__file__)))
  uncrustify_cfg_path = os.path.join(cfgPath.replace("/cygdrive/c", "c:"), uncrustify_cfg)

  # Get and filter staged files for commit
  stagedFiles = check_output(["git","diff-index","--cached","--diff-filter=ACMR","--name-only","HEAD", "--"]).decode("utf-8")
  stagedFiles=[file for file in stagedFiles.splitlines() if matchExt(file)]

  # Look for .uncrustify-ignore file at top level of repo
  rootRepoDir = check_output(["git", "rev-parse", "--show-toplevel"]).decode("utf-8").strip("\r\n")
  ignoreFilePath = os.path.join(rootRepoDir, ignore_file)

  # Collect ignore patterns (ignoring comments)
  ignorePatterns = []
  if os.path.exists(ignoreFilePath):
    with open(ignoreFilePath) as ignorefile:
      for line in ignorefile:
        if line and not line.startswith('#'):
          ignorePatterns.append(line.strip())

  # Cycle through files and check for matches with ignore patterns
  # (Assuming less ignore patterns than files for speed with large commits)
  skippedFiles = []
  for file in stagedFiles:
    for ignore in ignorePatterns:
      if fnmatch.fnmatch(file, ignore) and file not in skippedFiles:
        print("SKIP: " + file)
        skippedFiles.append(file)

  # Can't remove elements in list while iterating through,
  # so store and remove later
  for file in skippedFiles:
    stagedFiles.remove(file)

  # Run uncrustify on each file
  if len(stagedFiles) >= 8:
    # Use a worker pool with num_workers == cpu_count
    pool = multiprocessing.Pool(None)
    result = pool.map(uncrustifyFile, stagedFiles)
  else:
    # Not enough work to be done for multithreading to be worth it
    result = map(uncrustifyFile, stagedFiles)
  # Filter the input file list to get list of files that failed uncrustify check
  uncrustifyFiles = list(itertools.compress(stagedFiles, result))
  print("Checked", len(stagedFiles), "files, found", len(uncrustifyFiles), "errors")
  print("***********************************************************************")
  # Exit cleanly if no violations were found
  if (len(uncrustifyFiles)==0):
    print("Uncrustify check passed")
    if v1compat:
      sys.exit(0)
    else:
      return 0

  # Cross reference offending files with dirty files in workspace
  dirtyFiles = check_output(["git","diff-files","--name-only"]).decode("utf-8")
  dirtyFiles=[file for file in dirtyFiles.splitlines() if file in uncrustifyFiles]

  # Print out uncrustify commands for offending files for ease of use
  print("Uncrustify check failed--Aborting commit.\n")
  print("Run the following commands from the root directory of the repo to modify the files in place:")
  print(uncrustifyExe, uncrustify_cfg_path, file)
  for file in uncrustifyFiles:
    if file in dirtyFiles:
      print("*", end='')
    print("%s -c %s -l C --replace %s" % (uncrustifyExe,
                                          uncrustify_cfg_path,
                                          file))

  if (len(dirtyFiles)>0):
    print("\n* These files have unstaged changes in your workspace and may require additional attention")

  # Exit with error code to abort commit
  if v1compat:
    sys.exit(1)
  else:
    return 1

if __name__ == '__main__':
  ret = runUncrustifyCheck(v1compat=False)
  sys.exit(ret)