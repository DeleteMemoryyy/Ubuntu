4.54:
    Modified parts:
    ################ Fetch Stage     ###################################

    ## What address should instruction be fetched at
    int f_pc = [
        # Mispredicted branch.  Fetch at incremented PC
        M_icode == IJXX && M_ifun != UNCOND && M_Cnd : M_valE;
        # Completion of RET instruction.
        W_icode == IRET : W_valM;
        # Default: Use predicted value of PC
        1 : F_predPC;
    ];
    # Predict next value of PC
    int f_predPC = [
        f_icode == ICALL ||(f_icode==IJXX && f_ifun == UNCOND) : f_valC;
        1 : f_valP;
    ];

    ################ Execute Stage #####################################

    ## Select input A to ALU
    int aluA = [
        E_icode in { IRRMOVL, IOPL } : E_valA;
        E_icode in { IIRMOVL, IRMMOVL, IMRMOVL, IJXX } : E_valC;
        E_icode in { ICALL, IPUSHL } : -4;
        E_icode in { IRET, IPOPL } : 4;
        # Other instructions don't need ALU
    ];

    ## Select input B to ALU
    int aluB = [
        E_icode in { IRMMOVL, IMRMOVL, IOPL, ICALL, 
                IPUSHL, IRET, IPOPL } : E_valB;
        E_icode in { IRRMOVL, IIRMOVL, IJXX } : 0;
        # Other instructions don't need ALU
    ];

    ################ Pipeline Register Control #########################

    bool D_bubble =
        # Mispredicted branch
        (E_icode == IJXX && E_ifun != UNCOND && e_Cnd) ||
        # Stalling at fetch while ret passes through pipeline
        # but not condition for a load/use hazard
        !(E_icode in { IMRMOVL, IPOPL } && E_dstM in { d_srcA, d_srcB }) &&
        IRET in { D_icode, E_icode, M_icode };

    bool E_bubble =
        # Mispredicted branch
        (E_icode == IJXX && E_ifun != UNCOND && e_Cnd) ||
        # Conditions for a load/use hazard
        E_icode in { IMRMOVL, IPOPL } &&
        E_dstM in { d_srcA, d_srcB};

4.56:
    A:
    E_icode in { IMRMOVL, IPOPL } &&
    E_dstM in { d_srcA, d_srcB } && 
    !(D_icode in { IRMMOVL, IPUSHL } && 
    E_dstM == d_srcA);

    B:
    Modified parts:

    ################ Execute Stage #####################################

    int e_valA = [
        E_icode in { IRMMOVL, IPUSHL } &&
        M_dstM == E_srcA : m_valM;
        1 : E_valA;  # Use valA from stage pipe register
    ];

    ################ Pipeline Register Control #########################

    bool F_stall =
        # Conditions for a load/use hazard
        E_icode in { IMRMOVL, IPOPL } &&
        E_dstM in { d_srcA, d_srcB } && 
        !(D_icode in { IRMMOVL, IPUSHL } && 
        E_dstM == d_srcA) ||
        # Stalling at fetch while ret passes through pipeline
        IRET in { D_icode, E_icode, M_icode };

    bool D_stall = 
        # Conditions for a load/use hazard
        E_icode in { IMRMOVL, IPOPL } &&
        E_dstM in { d_srcA, d_srcB } && 
        !(D_icode in { IRMMOVL, IPUSHL } && 
        E_dstM == d_srcA); 

    bool D_bubble =
        # Mispredicted branch
        (E_icode == IJXX && !e_Cnd) ||
        # Stalling at fetch while ret passes through pipeline
        # but not condition for a load/use hazard
        !(E_icode in { IMRMOVL, IPOPL } && E_dstM in { d_srcA, d_srcB } && !(D_icode in { IRMMOVL, IPUSHL } && 
        E_dstM == d_srcA)) &&
        IRET in { D_icode, E_icode, M_icode };

    bool E_bubble =
        # Mispredicted branch
        (E_icode == IJXX && !e_Cnd) ||
        # Conditions for a load/use hazard
        E_icode in { IMRMOVL, IPOPL } &&
        E_dstM in { d_srcA, d_srcB } && 
        !(D_icode in { IRMMOVL, IPUSHL } && 
        E_dstM == d_srcA);

5.13:
    A:
    请见5.13_A_1.jpg和5.13_A_2.jpg

    B:
    lower bound of CPE: 3.0

    C:
    lower bound of CPE: 1.5

    D:
    因x86-64浮点乘法运算的capacity为2，即可允许两个乘法运算同时进行，因此限制CPE的便成了浮点的加法运算，最少需要CPE为3。

5.14:
    void inner5(vec_ptr u,vec_ptr v,data_t *dest)
    {
        long i;
        long length = vec_length(u);
        long limit = length - 5;
        data_t *udata = get_vec_start(u);
        data_t *vdata = get_vec_start(v);
        data_t sum = (data_t) 0;
        
        for(i =0;i<limit;i-=6)
        {
            sum += udata[i]*vdata[i]
                +  udata[i+1]*vdata[i+1]
                +  udata[i+2]*vdata[i+2]
                +  udata[i+3]*vdata[i+3]
                +  udata[i+4]*vdata[i+4]
                +  udata[i+5]*vdata[i+5];
        }

        for(;i<length;++i)
        {
            sum += udata[i]*vdata[i];
        }

        *dest = sum
    }    

    A:
    因为加载数据的latency为1，因此至少需要1个周期完成一次循环内容。
    B:
    因为浮点数运算的乘法器数量为1，单纯的展开（不创建平行变量）无法改变原本顺序的乘法运算，不能起到优化作用。
