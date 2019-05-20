def to_binary(dec):
    binar = ''
    while dec // 2 != 0:
        binar = str(dec % 2) + binar
        dec = dec // 2

    return str(dec) + binar

def to_decimal(binary): 
    binary1 = binary
    decimal, i, n = 0, 0, 0
    while(binary != 0): 
        dec = binary % 10
        decimal = decimal + dec * pow(2, i) 
        binary = binary//10
        i += 1
    
    return decimal

def binary_fixed(binar, siz):
    for x in range(len(binar),siz):
    	binar = '0' + binar

    return binar

def complete_str(istr, size):
    tstr = str(istr)
    for x in range(len(tstr), size):
        tstr = '0' + tstr

    return tstr;

def print_ips(sip_a, sm_a, sn_a, sb_a, s1, s2, s3):
    print("Ip B.".ljust(s1,' '), end="")
    for x in sip_a:
        print(x.center(s2,' '), end="")
    print()

    print("Mask B.".ljust(s1,' '), end="")
    for x in sm_a:
        print(x.center(s2,' '), end="")
    print()

    print("Network B.".ljust(s1,' '), end="")
    for x in sn_a:
        print(x.center(s2,' '), end="")
    print()

    print("Broadcast B.".ljust(s1,' '), end="")
    for x in sb_a:
        print(x.center(s2,' '), end="")
    print()


    print("-".center(s3,'-'), end="\n")


    print("Ip A.".ljust(s1,' '), end="")
    for x in sip_a:
        print(complete_str(to_decimal(int(x)), 3).center(s2,' '), end="")
    print()
    print("Mask A.".ljust(s1,' '), end="")
    for x in sm_a:
        print(complete_str(to_decimal(int(x)), 3).center(s2,' '), end="")
    print()
    print("Network A.".ljust(s1,' '), end="")
    for x in sn_a:
        print(complete_str(to_decimal(int(x)), 3).center(s2,' '), end="")
    print()
    print("Broadcast A.".ljust(s1,' '), end="")
    for x in sb_a:
        print(complete_str(to_decimal(int(x)), 3).center(s2,' '), end="")
    print("\n\n")