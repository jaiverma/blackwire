from .apps.hello import HelloApp

class Manager(Object):
    def __init__(self):
        self.apps = []

    def register_app(self, app, command):
        self.apps.append({
            'app': app,
            'cmd': command
        })
