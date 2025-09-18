#!/usr/bin/env python

###############################################################################
# meta_util.py
#
# Author(s): Maurizio Nanni
#            Tamas Hornos
#
# Description: Provides utility functions for accessing, checking and
#              manipulating modules meta information.
#
# Details: To list all available input arguments call 'meta_util.py -h'
#
# Note: Use public functions only, when imported as python module!
###############################################################################


import os
import sys
import yaml
import argparse
import subprocess


####################################################################################################
# Private Functions
####################################################################################################

def _printError(string):
  """Print error to stderr."""
  sys.stderr.write("[ERROR] %s\n" % string)


def _subprocessCall(args):
  try:
    return subprocess.check_output(args, stderr=subprocess.STDOUT)
  except subprocess.CalledProcessError as e:
    print (e.cmd)
    print (e.returncode)
    print (e.output)
    sys.exit(1)


def _getRepoRootAbsolutePath():
  """Get absolute path of Super root.

  This assumes that meta_util.py is located in Super root.

  Returns:
      Absolute path if successful, None otherwise.
  """
  return os.path.dirname(os.path.abspath(os.path.realpath(__file__)))


def _getModuleAbsolutePath(symbol):
  """Get absolute path of the module corresponding to the passed symbol.

  Returns:
      Absolute path if successful, None otherwise.
  """
  pyYamlMajorVersion= yaml.__version__.split(".")[0]
  
  if pyYamlMajorVersion>="5":
    modulesInfo = (yaml.load(open(MODULES_FILE, "r").read(), Loader=yaml.FullLoader))["module"]
  else:
    modulesInfo = (yaml.load(open(MODULES_FILE, "r").read()))["module"]
  
  for module in modulesInfo:
    if module["symbol"] == symbol:
      absPath = os.path.join(REPO_ROOT_DIR, module["path"])
      return absPath
  return None


####################################################################################################
# Globals
####################################################################################################

REPO_ROOT_DIR = _getRepoRootAbsolutePath()
META_INFO_DIR = os.path.join(REPO_ROOT_DIR, "meta")
MODULES_FILE = os.path.join(META_INFO_DIR, "modules.yaml")
PATH_TO_REPO_ROOT = os.path.relpath(REPO_ROOT_DIR)

RUN_DIAGNOSTIC = False


####################################################################################################
# Public Functions
####################################################################################################

def get_module_path(symbol):
  """Return the path of module relative to CWD.

  Args:
      symbol: symbol of module.

  Returns:
      Path relative to CWD if module found, None otherwise.
  """
  absCwdPath = os.path.normpath(os.getcwd())
  absModulePath = _getModuleAbsolutePath(symbol)
  if absModulePath is None:
    _printError("Symbol not found: %s!" % symbol)
    return None
  return os.path.relpath(absModulePath, absCwdPath)


def get_module_rel_path(symbol, fromPath):
  """Return path of module relative to querying path.

  Args:
      symbol: symbol of module.
      fromPath: starting point of the requested relative path.

  Returns:
      Path relative to querying path if module version found, None otherwise.
  """
  absCwdPath = os.path.normpath(os.getcwd())
  absModulePath = _getModuleAbsolutePath(symbol)
  if absModulePath is None:
    _printError("Symbol not found: %s!" % symbol)
    return None
  return os.path.relpath(absModulePath, fromPath)


####################################################################################################
# Test All Public Functions and more
####################################################################################################

def _runSanityCheckTest():
  print ("TODO")

tests = {
  "sanity_check"   : _runSanityCheckTest,
}


def run_test(test_name):
  """Run test."""
  err = 0
  name = test_name
  if name in tests:
    print ("[Testing %s" % (name)),
    tests[name]()
    print ("done]")
  else :
    print ("error: unknown test name " + name)
    err = 1
  return err


####################################################################################################
# Main
####################################################################################################

def main():

  parser = argparse.ArgumentParser(
    description="Utility script for accessing module meta information.")
  subParser = parser.add_subparsers()

  # Command "get_module_path"
  # Arguments:
  #            symbol
  getModulePathCommand = subParser.add_parser(
    "get_module_path",
    help="Takes a module symbol and returns the relative path to the module \
          corresponding to the passed symbol")
  getModulePathCommand.set_defaults(command="get_module_path")
  getModulePathCommand.add_argument("symbol",
                                    type=str,
                                    help="The symbol of the requested module")

  # Command "get_dependency_path"
  # Arguments:
  #            symbol
  # This is a legacy CLI command, now deprecated. Use get_module_path instead.
  getDependencyPathCommand = subParser.add_parser(
    "get_dependency_path",
    help="Takes a module symbol and returns the relative path to the module \
          corresponding to the passed symbol. This is deprecated, use \
          get_module_path instead")
  getDependencyPathCommand.set_defaults(command="get_dependency_path")
  getDependencyPathCommand.add_argument("symbol",
                                    type=str,
                                    help="The symbol of the requested module")


  # Command "get_module_rel_path"
  # Arguments:
  #            symbol
  #            from_path
  getModuleRelPathCommand = subParser.add_parser(
    "get_module_rel_path",
     help="Takes a module symbol and a path and returns the relative path to \
           the module corresponding to the passed symbol from the passed path")
  getModuleRelPathCommand.set_defaults(command="get_module_rel_path")
  getModuleRelPathCommand.add_argument("symbol",
                                       type=str,
                                       help="The symbol of the requested module")
  getModuleRelPathCommand.add_argument("from_path",
                                       type=str,
                                       help="The start point of the relative path")

  # Command "run_test"
  # Arguments:
  #            test_name
  runTestCommand = subParser.add_parser(
    "run_test",
    help="Run a test to verify the submodule metadata for Super")
  runTestCommand.set_defaults(command="run_test")
  runTestCommand.add_argument("test_name",
                              type=str,
                              choices=list(tests.keys()),
                              help="The name of the test to run")

  # Parse input
  options = parser.parse_args()

  # Command "get_module_path"
  if options.command == "get_module_path":
    path = get_module_path(options.symbol)
    if path is None:
      sys.exit(1)
    print (path)

  # Command "get_module_path"
  elif options.command == "get_dependency_path":
    path = get_module_path(options.symbol)
    if path is None:
      sys.exit(1)
    print (path)

  # Command "get_module_rel_path"
  elif options.command == "get_module_rel_path":
    path = get_module_rel_path(options.symbol, options.from_path)
    if path is None:
      sys.exit(1)
    print (path)

  # Command "run_test"
  elif options.command == "run_test":
    run_test(options.test_name)


if __name__ == "__main__":
  main()