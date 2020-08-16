import json

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
                setattr(cls, func_name + '_' + str(i), tmp())
        # Remove func from class:
        setattr(cls, func_name, None)
        return cls
    return decorator