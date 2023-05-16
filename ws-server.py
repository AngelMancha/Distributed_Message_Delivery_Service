from spyne import Application, ServiceBase, Unicode, rpc
from spyne.protocol.soap import Soap11
from spyne.server.wsgi import WsgiApplication


class Server(ServiceBase):

    @rpc(Unicode, _returns=Unicode)
    def modify_text(ctx, mensaje):
        print("Estoy en modify_message")
        print(f"Mensaje {mensaje} recibido")
        # Reemplazar múltiples espacios en blanco por un solo espacio en blanco
        mensaje_modificado = " ".join(mensaje.split())
        print(f"Mensaje {mensaje_modificado} enviado")

        return mensaje_modificado

application = Application(
    services=[Server],
    tns='http://tests.python-zeep.org/',
    in_protocol=Soap11(validator='lxml'),
    out_protocol=Soap11())

application = WsgiApplication(application)

if __name__ == '__main__':
    import logging

    from wsgiref.simple_server import make_server

    logging.basicConfig(level=logging.DEBUG)
    logging.getLogger('spyne.protocol.xml').setLevel(logging.DEBUG)

    logging.info("listening to http://localhost:5000")
    logging.info("wsdl is at: http://localhost:5000/?wsdl")

    server = make_server('127.0.0.1', 5000, application)
    server.serve_forever()