import subprocess as sp
import sys
from getpass import getpass
import telnetlib
import multiprocessing as mp
import os
import threading
import unittest
import tempfile
from helper import create_tests, cases_from_files

interpreterPath = "../build/overstacked"
inputCasePath = "/input.txt"
outputCasePath = "/output.txt"

@create_tests('test_operators', 'simpleCases.json')
class SimpleTests(unittest.TestCase):
    def test_operators(self, input, expectedOutput):
        f = tempfile.NamedTemporaryFile(delete=False)
        f.write(input.encode("utf-8"))
        f.close()
        inputFilePath = f.name
        proc = sp.Popen([interpreterPath, inputFilePath], stdout=sp.PIPE)
        (output, err) = proc.communicate()
        proc.wait()
        #print(f"[ {input} ] => {expectedOutput} ?")
        assert output.decode("utf-8") == expectedOutput, f"{output.decode('utf-8')} != {expectedOutput}"
        os.unlink(f.name)

@cases_from_files('test_files')
class TestsFromFiles(unittest.TestCase):
    def test_files(self, case_folder):
        inputFilePath = case_folder + inputCasePath
        proc = sp.Popen([interpreterPath, inputFilePath], stdout=sp.PIPE)
        (output, err) = proc.communicate()
        proc.wait()
        expectedOutputFile = open(case_folder + outputCasePath, 'r')
        expectedOutput = expectedOutputFile.read()
        expectedOutputFile.close()
        assert output.decode("utf-8") == expectedOutput, f"{output.decode('utf-8')} != {expectedOutput}"


if __name__ == "__main__":
    unittest.main()
