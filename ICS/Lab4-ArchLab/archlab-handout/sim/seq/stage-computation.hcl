# Stage Computation:leave

    # leave
Fetch:
    icode-ifun ← M[PC]
    valP ← PC + 1
Decode:
    valB ← R[%ebp]
Execute:
    valE ← valB + 4
Memory:
    valM ← M[valB]
Write Back:
    R[%esp] ← valE
    R[%ebp] ← valM
PC Update:
    PC ← valP


# Stage Computation:iaddl

    # iddl V,rB
Fetch:
    icode-ifun ← M[PC]
    rA-rB ← M[PC+1]
    valC ← M[PC+2]
    valP  ← PC + 6
Decode:
    valB ← R[rB]
Execute:
    valE ← valB + valC
    Set CC
Memory:
Write Back:
    R[rB] ← valE
PC Update:
    PC ← valP
