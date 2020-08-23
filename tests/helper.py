import json
import glob

complexCasesPath = "cases/*"
simpleCasesPath = "simpleCases/*"

def create_tests(func_name, file):
    def decorator(cls):
        func = getattr(cls, func_name)
        # Set new funcs to class:
        with open(file, 'r') as fh:
            data = json.loads(fh.read())['data']
            for i, params in enumerate(data):
                def tmp(params=params):  # need for http://stackoverflow.com/q/7546285/1113207
                    def wrapper(self, *args, **kwargs):
                        return func(self, **params)
                    return wrapper
                setattr(cls, func_name + '_' + params["name"], tmp())
        # Remove func from class:
        setattr(cls, func_name, None)
        return cls
    return decorator

def cases_from_files(func_name):
    def decorator(cls):
        func = getattr(cls, func_name)
        # Set new funcs to class:
        # print("cases: ", glob.glob("cases/*"))
        casesFolders = glob.glob(complexCasesPath)
        for i, params in enumerate(casesFolders):
            def tmp(params=params):  # need for http://stackoverflow.com/q/7546285/1113207
                def wrapper(self, *args, **kwargs):
                    return func(self, params)
                return wrapper
            setattr(cls, func_name + '_' + str(params.replace('\\','/').split('/')[1]), tmp())
        # Remove func from class:
        setattr(cls, func_name, None)
        return cls
    return decorator

def create_simple_cases(func_name):
    def decorator(cls):
        func = getattr(cls, func_name)
        casesFiles = glob.glob(simpleCasesPath)
        # Set new funcs to class:
        for file in casesFiles:
            with open(file, 'r') as fh:
                data = json.loads(fh.read())['cases']
                for i, params in enumerate(data):
                    def tmp(params=params):  # need for http://stackoverflow.com/q/7546285/1113207
                        def wrapper(self, *args, **kwargs):
                            return func(self, **params)
                        return wrapper
                    setattr(cls, func_name + '_' + file.replace('\\','/').split('/')[1].split('.')[0] + '_' + params["name"], tmp())
            # Remove func from class:
            setattr(cls, func_name, None)
        return cls
    return decorator