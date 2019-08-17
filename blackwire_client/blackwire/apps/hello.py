from .base import BaseApp

class HelloApp(BaseApp):
    def __init__(self):
        BaseApp.__init__()
        print('[*] SampleApp constructor called')

    def run(self):
        print('Hello World!')
