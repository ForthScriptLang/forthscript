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

interpreterPath = "../build/overstacked"

@create_tests('test_operators', 'simpleCases.json')
class Test(unittest.TestCase):
    def test_operators(self, input, expectedOutput):
        f = tempfile.NamedTemporaryFile(delete=False)
        f.write(input.encode("utf-8"))
        f.close()
        inputFilePath = f.name
        proc = sp.Popen([interpreterPath, inputFilePath], stdout=sp.PIPE)
        (output, err) = proc.communicate()
        proc.wait()
        print(f"[ {input} ] => {expectedOutput} ?")
        assert output.decode("utf-8") == expectedOutput, f"{output.decode('utf-8')} != {expectedOutput}"
        os.unlink(f.name)

if __name__ == "__main__":
    unittest.main()
