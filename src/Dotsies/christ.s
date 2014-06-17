ldr r0, =0x20200000
mov r1, #1
lsl r1, #12
str r1, [r0, #0]
mov r2, #1
lsl r2, #21
str r2, [r0, #4]
mov r3, #0x1000000
orr r3, r3, r2
str r3, [r0, #4]
mov r4, #1
lsl r4, #6
str r4, [r0, #8]
mov r5, #0x200000
orr r5, r5, r4
str r5, [r0, #8]
mov r1, #1
lsl r1, #4
mov r2, #1
lsl r2, #17
mov r3, #1
lsl r3, #18
mov r4, #1
lsl r4, #22
mov r5, #1
lsl r5, #27
mov r10, #1
mov r12, #0
str r1, [r0, #40]
str r2, [r0, #40]
str r3, [r0, #40]
str r4, [r0, #40]
str r5, [r0, #40]
mov r9, #0xFF0000
waitloop0:
sub r9, r9, #1
cmp r9, #0
bne waitloop0
str r1, [r0, #28]
str r2, [r0, #28]
str r5, [r0, #28]
mov r9, #0xFF0000
waitloop1:
sub r9, r9, #1
cmp r9, #0
bne waitloop1
str r1, [r0, #40]
str r2, [r0, #40]
str r3, [r0, #40]
str r4, [r0, #40]
str r5, [r0, #40]
mov r9, #0xFF0000
waitloop2:
sub r9, r9, #1
cmp r9, #0
bne waitloop2
str r5, [r0, #28]
mov r9, #0xFF0000
waitloop3:
sub r9, r9, #1
cmp r9, #0
bne waitloop3
str r1, [r0, #40]
str r2, [r0, #40]
str r3, [r0, #40]
str r4, [r0, #40]
str r5, [r0, #40]
mov r9, #0xFF0000
waitloop4:
sub r9, r9, #1
cmp r9, #0
bne waitloop4
mov r9, #0xFF0000
waitloop5:
sub r9, r9, #1
cmp r9, #0
bne waitloop5
str r1, [r0, #40]
str r2, [r0, #40]
str r3, [r0, #40]
str r4, [r0, #40]
str r5, [r0, #40]
mov r9, #0xFF0000
waitloop6:
sub r9, r9, #1
cmp r9, #0
bne waitloop6
str r1, [r0, #28]
mov r9, #0xFF0000
waitloop7:
sub r9, r9, #1
cmp r9, #0
bne waitloop7
str r1, [r0, #40]
str r2, [r0, #40]
str r3, [r0, #40]
str r4, [r0, #40]
str r5, [r0, #40]
mov r9, #0xFF0000
waitloop8:
sub r9, r9, #1
cmp r9, #0
bne waitloop8
str r1, [r0, #28]
str r3, [r0, #28]
str r4, [r0, #28]
mov r9, #0xFF0000
waitloop9:
sub r9, r9, #1
cmp r9, #0
bne waitloop9
str r1, [r0, #40]
str r2, [r0, #40]
str r3, [r0, #40]
str r4, [r0, #40]
str r5, [r0, #40]
mov r9, #0xFF0000
waitloop10:
sub r9, r9, #1
cmp r9, #0
bne waitloop10
str r5, [r0, #28]
mov r9, #0xFF0000
waitloop11:
sub r9, r9, #1
cmp r9, #0
bne waitloop11
str r1, [r0, #40]
str r2, [r0, #40]
str r3, [r0, #40]
str r4, [r0, #40]
str r5, [r0, #40]
mov r9, #0xFF0000
waitloop12:
sub r9, r9, #1
cmp r9, #0
bne waitloop12
mov r9, #0xFF0000
waitloop13:
sub r9, r9, #1
cmp r9, #0
bne waitloop13
str r1, [r0, #40]
str r2, [r0, #40]
str r3, [r0, #40]
str r4, [r0, #40]
str r5, [r0, #40]
mov r9, #0xFF0000
waitloop14:
sub r9, r9, #1
cmp r9, #0
bne waitloop14
str r1, [r0, #28]
mov r9, #0xFF0000
waitloop15:
sub r9, r9, #1
cmp r9, #0
bne waitloop15
str r1, [r0, #40]
str r2, [r0, #40]
str r3, [r0, #40]
str r4, [r0, #40]
str r5, [r0, #40]
mov r9, #0xFF0000
waitloop16:
sub r9, r9, #1
cmp r9, #0
bne waitloop16
str r1, [r0, #28]
str r2, [r0, #28]
str r5, [r0, #28]
mov r9, #0xFF0000
waitloop17:
sub r9, r9, #1
cmp r9, #0
bne waitloop17
str r1, [r0, #40]
str r2, [r0, #40]
str r3, [r0, #40]
str r4, [r0, #40]
str r5, [r0, #40]
mov r9, #0xFF0000
waitloop18:
sub r9, r9, #1
cmp r9, #0
bne waitloop18
str r5, [r0, #28]
mov r9, #0xFF0000
waitloop19:
sub r9, r9, #1
cmp r9, #0
bne waitloop19
str r1, [r0, #40]
str r2, [r0, #40]
str r3, [r0, #40]
str r4, [r0, #40]
str r5, [r0, #40]
mov r9, #0xFF0000
waitloop20:
sub r9, r9, #1
cmp r9, #0
bne waitloop20
str r2, [r0, #28]
str r3, [r0, #28]
str r4, [r0, #28]
mov r9, #0xFF0000
waitloop21:
sub r9, r9, #1
cmp r9, #0
bne waitloop21
str r1, [r0, #40]
str r2, [r0, #40]
str r3, [r0, #40]
str r4, [r0, #40]
str r5, [r0, #40]
mov r9, #0xFF0000
waitloop22:
sub r9, r9, #1
cmp r9, #0
bne waitloop22
str r1, [r0, #28]
str r5, [r0, #28]
mov r9, #0xFF0000
waitloop23:
sub r9, r9, #1
cmp r9, #0
bne waitloop23
str r1, [r0, #40]
str r2, [r0, #40]
str r3, [r0, #40]
str r4, [r0, #40]
str r5, [r0, #40]
mov r9, #0xFF0000
waitloop24:
sub r9, r9, #1
cmp r9, #0
bne waitloop24
str r1, [r0, #28]
str r4, [r0, #28]
mov r9, #0xFF0000
waitloop25:
sub r9, r9, #1
cmp r9, #0
bne waitloop25
str r1, [r0, #40]
str r2, [r0, #40]
str r3, [r0, #40]
str r4, [r0, #40]
str r5, [r0, #40]
mov r9, #0xFF0000
waitloop26:
sub r9, r9, #1
cmp r9, #0
bne waitloop26
str r5, [r0, #28]
mov r9, #0xFF0000
waitloop27:
sub r9, r9, #1
cmp r9, #0
bne waitloop27
str r1, [r0, #40]
str r2, [r0, #40]
str r3, [r0, #40]
str r4, [r0, #40]
str r5, [r0, #40]
mov r9, #0xFF0000
waitloop28:
sub r9, r9, #1
cmp r9, #0
bne waitloop28
mov r9, #0xFF0000
waitloop29:
sub r9, r9, #1
cmp r9, #0
bne waitloop29
str r1, [r0, #40]
str r2, [r0, #40]
str r3, [r0, #40]
str r4, [r0, #40]
str r5, [r0, #40]
mov r9, #0xFF0000
waitloop30:
sub r9, r9, #1
cmp r9, #0
bne waitloop30
str r5, [r0, #28]
mov r9, #0xFF0000
waitloop31:
sub r9, r9, #1
cmp r9, #0
bne waitloop31
str r1, [r0, #40]
str r2, [r0, #40]
str r3, [r0, #40]
str r4, [r0, #40]
str r5, [r0, #40]
mov r9, #0xFF0000
waitloop32:
sub r9, r9, #1
cmp r9, #0
bne waitloop32
str r1, [r0, #28]
str r3, [r0, #28]
str r5, [r0, #28]
mov r9, #0xFF0000
waitloop33:
sub r9, r9, #1
cmp r9, #0
bne waitloop33
str r1, [r0, #40]
str r2, [r0, #40]
str r3, [r0, #40]
str r4, [r0, #40]
str r5, [r0, #40]
mov r9, #0xFF0000
waitloop34:
sub r9, r9, #1
cmp r9, #0
bne waitloop34
str r1, [r0, #28]
str r2, [r0, #28]
str r3, [r0, #28]
mov r9, #0xFF0000
waitloop35:
sub r9, r9, #1
cmp r9, #0
bne waitloop35
str r1, [r0, #40]
str r2, [r0, #40]
str r3, [r0, #40]
str r4, [r0, #40]
str r5, [r0, #40]
mov r9, #0xFF0000
waitloop36:
sub r9, r9, #1
cmp r9, #0
bne waitloop36
str r5, [r0, #28]
mov r9, #0xFF0000
waitloop37:
sub r9, r9, #1
cmp r9, #0
bne waitloop37
str r1, [r0, #40]
str r2, [r0, #40]
str r3, [r0, #40]
str r4, [r0, #40]
str r5, [r0, #40]
mov r9, #0xFF0000
waitloop38:
sub r9, r9, #1
cmp r9, #0
bne waitloop38
str r3, [r0, #28]
mov r9, #0xFF0000
waitloop39:
sub r9, r9, #1
cmp r9, #0
bne waitloop39
str r1, [r0, #40]
str r2, [r0, #40]
str r3, [r0, #40]
str r4, [r0, #40]
str r5, [r0, #40]
mov r9, #0xFF0000
waitloop40:
sub r9, r9, #1
cmp r9, #0
bne waitloop40
str r2, [r0, #28]
str r3, [r0, #28]
str r5, [r0, #28]
mov r9, #0xFF0000
waitloop41:
sub r9, r9, #1
cmp r9, #0
bne waitloop41
str r1, [r0, #40]
str r2, [r0, #40]
str r3, [r0, #40]
str r4, [r0, #40]
str r5, [r0, #40]
mov r9, #0xFF0000
waitloop42:
sub r9, r9, #1
cmp r9, #0
bne waitloop42
mov r9, #0xFF0000
waitloop43:
sub r9, r9, #1
cmp r9, #0
bne waitloop43
str r1, [r0, #40]
str r2, [r0, #40]
str r3, [r0, #40]
str r4, [r0, #40]
str r5, [r0, #40]
mov r9, #0xFF0000
waitloop44:
sub r9, r9, #1
cmp r9, #0
bne waitloop44
str r2, [r0, #28]
str r4, [r0, #28]
str r5, [r0, #28]
mov r9, #0xFF0000
waitloop45:
sub r9, r9, #1
cmp r9, #0
bne waitloop45
str r1, [r0, #40]
str r2, [r0, #40]
str r3, [r0, #40]
str r4, [r0, #40]
str r5, [r0, #40]
mov r9, #0xFF0000
waitloop46:
sub r9, r9, #1
cmp r9, #0
bne waitloop46
str r2, [r0, #28]
str r3, [r0, #28]
str r4, [r0, #28]
mov r9, #0xFF0000
waitloop47:
sub r9, r9, #1
cmp r9, #0
bne waitloop47
str r1, [r0, #40]
str r2, [r0, #40]
str r3, [r0, #40]
str r4, [r0, #40]
str r5, [r0, #40]
mov r9, #0xFF0000
waitloop48:
sub r9, r9, #1
cmp r9, #0
bne waitloop48
mov r9, #0xFF0000
waitloop49:
sub r9, r9, #1
cmp r9, #0
bne waitloop49
