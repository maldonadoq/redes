from utils import to_binary, to_decimal, binary_fixed, complete_str, print_ips

def next_ip(broadcast):
	tinit = ""
	for x in broadcast:
		tinit = tinit + x
	
	tdec = to_decimal(int(tinit))
	tbin = binary_fixed(to_binary(tdec+1), 32)

	tinit = ""
	tmp = ""
	for i in range(0,len(tbin)):
		if(i%8) == 0 and i!=0:			
			tinit = tinit + str(to_decimal(int(tmp))) + "."
			tmp = ""
		tmp = tmp + tbin[i]
	tinit = tinit + str(to_decimal(int(tmp)))

	return tinit

def calculate_address(pcs, init, size, ipsize):
	ipinit = init
	pcs.sort(reverse = True)

	for ip in pcs:
		ip_part = ipinit.split('.')
		tmp_size = len(to_binary(ip))

		if ip == tmp_size:
			tmp_size = tmp_size - 1

		tm = ipsize - tmp_size

		ip_a = []
		m_a  = []
		n_a  = []

		# ip bits
		# print(ip_part)
		for ipp in ip_part:
			tb = binary_fixed(to_binary(int(ipp)), size)
			# print(tb)
			for i in range(0, size):
				ip_a.append(tb[i])
		# mac bits
		for i in range(0, ipsize):
			if(i < tm):
				m_a.append(1)
			else:
				m_a.append(0)

		# na bits
		for i in range(0, ipsize):
			n_a.append(int(ip_a[i]) and m_a[i])

		# ba bits
		b_a = ip_a.copy()
		for i in range(tm,ipsize):
			b_a[i] = 1


		# to decimal
		sip_a = []
		sm_a  = []
		sn_a  = []
		sb_a  = []
		
		sip = ""
		sm = ""
		sn = ""
		sb = ""

		for i in range(0,ipsize):
			if(i%size) == 0 and i!=0:
				sip_a.append(sip)
				sip = ""
				sm_a.append(sm)
				sm = ""				
				sn_a.append(sn)
				sn = ""				
				sb_a.append(sb)
				sb = ""				
			sip = sip + str(ip_a[i])
			sm  = sm  + str(m_a[i])
			sn  = sn  + str(n_a[i])
			sb  = sb  + str(b_a[i])

		sip_a.append(sip)
		sm_a.append(sm)
		sn_a.append(sn)
		sb_a.append(sb)

		print_ips(sip_a, sm_a, sn_a, sb_a, 13, size+2, ipsize)
		ipinit = next_ip(sb_a)

def main():
	# private
	# pcp = [180, 18, 82, 20]
	# init = "10.0.0.0"	

	# public
	pcp  = [2, 56, 198, 560, 12]	
	init = "128.0.0.0"

	size = 8
	ipsize = 32

	calculate_address(pcp, init, size, ipsize)

if __name__ == "__main__": 	
    main()