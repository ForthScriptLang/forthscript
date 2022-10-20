import subprocess as sp
import sys
from getpass import getpass
import telnetlib
import multiprocessing as mp
import os
import threading
import unittest
import tempfile
import glob
from helper import create_tests, cases_from_files, create_simple_cases

interpreterPath = "../build/forthscript"
inputCasePath = "/main.fscript"
outputCasePath = "/expectedOutput.txt"

# Print "../build" directory contents
print('../build directory contains these files:')
for filepath in glob.iglob('../build/*'):
    print(f'* {filepath}')


# remove this with simpleCases.json and create_tests
@create_tests('test_old', 'simpleCases.json')
class SimpleTestsOld(unittest.TestCase):
    def test_old(self, name, input, expectedOutput):
        f = tempfile.NamedTemporaryFile(delete=False)
        f.write(input.encode("utf-8"))
        f.close()
        inputFilePath = f.name
        proc = sp.Popen([interpreterPath, inputFilePath], stdout=sp.PIPE)
        (output, err) = proc.communicate()
        proc.wait()
        actualOutput = output.decode("utf-8").replace('\r', '')
        #print(f"[ {input} ] => {expectedOutput} ?")
        assert repr(actualOutput) == repr(
            expectedOutput), f"{repr(actualOutput)} != {repr(expectedOutput)}"
        os.unlink(f.name)


@cases_from_files('test_files')
class TestsFromFiles(unittest.TestCase):
    def test_files(self, case_folder):
        inputFilePath = case_folder + inputCasePath
        proc = sp.Popen([interpreterPath, inputFilePath], stdout=sp.PIPE)
        (output, err) = proc.communicate()
        proc.wait()
        actualOutput = output.decode("utf-8").replace('\r', '')
        expectedOutputFile = open(case_folder + outputCasePath, 'r')
        expectedOutput = expectedOutputFile.read()
        expectedOutputFile.close()
        assert repr(actualOutput) == repr(
            expectedOutput), f"{repr(actualOutput)} != {repr(expectedOutput)}"


@create_simple_cases('test')
class SimpleTests(unittest.TestCase):
    def test(self, name, input, expectedOutput):
        f = tempfile.NamedTemporaryFile(delete=False)
        f.write(input.encode("utf-8"))
        f.close()
        inputFilePath = f.name
        proc = sp.Popen([interpreterPath, inputFilePath], stdout=sp.PIPE)
        (output, err) = proc.communicate()
        proc.wait()
        actualOutput = output.decode("utf-8").replace('\r', '')
        #print(f"[ {input} ] => {expectedOutput} ?")
        assert repr(actualOutput) == repr(
            expectedOutput), f"{repr(actualOutput)} != {repr(expectedOutput)}"
        os.unlink(f.name)


if __name__ == "__main__":
    unittest.main()
