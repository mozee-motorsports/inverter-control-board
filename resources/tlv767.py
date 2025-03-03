Vout = 12
Vfb = 0.8
Ifb = 50e-9

R1R2 = (Vout/Vfb) - 1
x = 1/R1R2


R2 = (x*(Vout/(Ifb*100)))/(1+x) # R2 upper bound


R1 = R2*(Vout/Vfb - 1)

print(f"R2 <= {R2:.2f}")
print(f"R1 = {R1:.2f}")



