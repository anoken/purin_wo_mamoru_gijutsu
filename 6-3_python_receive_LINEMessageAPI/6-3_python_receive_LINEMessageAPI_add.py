from pythonosc import osc_message_builder
from pythonosc import udp_client
port_num = 10000
parser = argparse.ArgumentParser()
parser.add_argument("--ip",default="m5stack_ip_address",help="Message Server")
parser.add_argument("--port",type=int,default=port_num,
	help="The port the OSC server is listening on")
args = parser.parse_args()
client = udp_client.SimpleUDPClient(args.ip,args.port)

/* LINE Message API�����b�Z�[�W����M�������*/
@handler.add(MessageEvent,message=TextMessage)
def handle_text_message(event):

�@�@/* LINE �̃��b�Z�[�W���R���\�[���ɕ\������*/
    text = event.message.text
    cmd = 'talk ' + text
    print(cmd)
    text2 ='speak to message'

    /* OSC��M5Stack�ցA���b�Z�[�W�𑗐M���� */
    client.send_message("/msg",cmd)

    /* LINE�Ɂuspeak to message�v�Ɗ����������Ƃ�ʒm����*/
    line_bot_api.reply_message(event.reply_token,TextSendMessage(text=text2))
