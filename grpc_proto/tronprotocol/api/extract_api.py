import os

class CodeGen:
	wallet_rpc = []
	wallet_request = []
	status = 0
	
	def __init__(self, path):
		file = open(path)
		for line in file.readlines():
			self.findRpc(line)


	def findRpc(self, line):
		
		token = line.split(' ')
		tokenCount = len(token)
		#print line
		#print tokenCount
		if self.status == 0:
			if tokenCount > 2 and token[0] == 'service' and token[1] == 'Wallet':
				self.status = 1
				#print "change status to 1"
				
		if self.status == 1:
			if tokenCount == 1 and token[0][0:2] == '};':
				self.status = 0
			else:
				t = line.lstrip().split(' ')
				if len(t) > 1 and t[0] == 'rpc':
					#print "find rpc"
					t[2] = self.trimBracket(t[2])
					t[4] = self.trimBracket(t[4])
					self.wallet_rpc.append(t)
					#print line
			
	def trimBracket(self, token):
		return token[1:-1].strip()
			
	def genEnum(self):
		for t in self.wallet_rpc:
			print 'Request_' + t[1] + ',' 
			
	def genHeader(self):
		for t in self.wallet_rpc:
			print 'int %s(std::shared_ptr<protocol::%s> %s, std::function<void(std::shared_ptr<protocol::%s>)> callback);' % (t[1], t[2], t[2].lower(), t[4])
			
	def genBody(self):
		for t in self.wallet_rpc:
			print "int CNetworkThread::%s(std::shared_ptr<protocol::%s> %s, std::function<void(std::shared_ptr<protocol::%s>)> callback)\n{\n  RPC_PROC(%s, %s, protocol::%s, %s);\n}\n" % (t[1], t[2], t[2].lower(), t[4], 'Request_'+t[1], t[2].lower(), t[4], t[1])



cg = CodeGen(os.getcwd()+'/api.proto')
cg.genEnum()
cg.genHeader()
cg.genBody()