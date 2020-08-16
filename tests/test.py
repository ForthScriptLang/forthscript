import subprocess as sp
import sys
from getpass import getpass
import telnetlib
import multiprocessing as mp
import os
import threading
import unittest
import tempfile
from helper import create_tests

interpreterPath = "../build/overstacked.exe"

@create_tests('test_operators', 'simpleCases.json')
class Test(unittest.TestCase):
    def test_operators(self, input, expectedOutput):
        f = tempfile.NamedTemporaryFile(delete=False)
        f.write(input.encode("utf-8"))
        #print("inputEncoded: ", input.encode("utf-8"))
        #print("input: ", input)
        f.close()
        inputFilePath = f.name
        #print("inputFilePath: ", inputFilePath)
        command = f"{interpreterPath} {inputFilePath}"
        proc = sp.Popen(command, stdout=sp.PIPE)
        (output, err) = proc.communicate()
        proc.wait()
        print(f"[ {input} ] => {expectedOutput} ?")
        assert output.decode("utf-8") == expectedOutput, f"{output.decode('utf-8')} != {expectedOutput}"
        os.unlink(f.name)

if __name__ == "__main__":
    unittest.main()
