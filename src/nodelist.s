	.file	"nodelist.c"
	.version	"01.01"
gcc2_compiled.:
.section	.rodata
.LC0:
	.ascii	"                Another Glitch in the Call\n                "
	.ascii	"------- ------ -- --- ----\n        (Sung to the tune of a r"
	.ascii	"ecent Pink Floyd song.)\nWe don't need no indirection\nWe do"
	.ascii	"n"
	.string	"'t need no flow control\nNo data typing or declarations\nDid you leave the lists alone?\n        Hey!  Hacker!  Leave those lists alone!\nChorus:\n        All in all, it's just a pure-LISP function call.\n        All in all, it's just a pure-LISP function call.\n"
.data
	.align 4
	.type	 nodelist_quote,@object
	.size	 nodelist_quote,4
nodelist_quote:
	.long .LC0
.text
	.align 4
.globl ListInit
	.type	 ListInit,@function
ListInit:
	movl 4(%esp),%eax
	leal 4(%eax),%edx
	movl %edx,(%eax)
	movl $0,4(%eax)
	movl %eax,8(%eax)
	ret
.Lfe1:
	.size	 ListInit,.Lfe1-ListInit
	.align 4
.globl ListAddHead
	.type	 ListAddHead,@function
ListAddHead:
	pushl 8(%esp)
	pushl 8(%esp)
	call NodeInsert
	addl $8,%esp
	ret
.Lfe2:
	.size	 ListAddHead,.Lfe2-ListAddHead
	.align 4
.globl ListAddTail
	.type	 ListAddTail,@function
ListAddTail:
	movl 4(%esp),%eax
	pushl 8(%esp)
	pushl 8(%eax)
	call NodeInsert
	addl $8,%esp
	ret
.Lfe3:
	.size	 ListAddTail,.Lfe3-ListAddTail
	.align 4
.globl ListGetHead
	.type	 ListGetHead,@function
ListGetHead:
	movl 4(%esp),%eax
	movl (%eax),%eax
	ret
.Lfe4:
	.size	 ListGetHead,.Lfe4-ListGetHead
	.align 4
.globl ListGetTail
	.type	 ListGetTail,@function
ListGetTail:
	movl 4(%esp),%eax
	movl 8(%eax),%eax
	ret
.Lfe5:
	.size	 ListGetTail,.Lfe5-ListGetTail
	.align 4
.globl ListRemHead
	.type	 ListRemHead,@function
ListRemHead:
	pushl %ebx
	pushl 8(%esp)
	call ListGetHead
	movl %eax,%ebx
	pushl %ebx
	call NodeValid
	addl $8,%esp
	testl %eax,%eax
	je .L7
	pushl %ebx
	call NodeRemove
	addl $4,%esp
.L7:
	movl %ebx,%eax
	popl %ebx
	ret
.Lfe6:
	.size	 ListRemHead,.Lfe6-ListRemHead
	.align 4
.globl ListRemTail
	.type	 ListRemTail,@function
ListRemTail:
	pushl %ebx
	pushl 8(%esp)
	call ListGetTail
	movl %eax,%ebx
	pushl %ebx
	call NodeValid
	addl $8,%esp
	testl %eax,%eax
	je .L9
	pushl %ebx
	call NodeRemove
	addl $4,%esp
.L9:
	movl %ebx,%eax
	popl %ebx
	ret
.Lfe7:
	.size	 ListRemTail,.Lfe7-ListRemTail
	.align 4
.globl ListEmpty
	.type	 ListEmpty,@function
ListEmpty:
	movl 4(%esp),%eax
	leal 4(%eax),%edx
	cmpl %edx,(%eax)
	sete %al
	andl $255,%eax
	ret
.Lfe8:
	.size	 ListEmpty,.Lfe8-ListEmpty
	.align 4
.globl NodeInsert
	.type	 NodeInsert,@function
NodeInsert:
	movl 4(%esp),%eax
	movl 8(%esp),%edx
	movl (%eax),%ecx
	movl %ecx,(%edx)
	movl %eax,4(%edx)
	movl %edx,(%eax)
	movl %edx,4(%ecx)
	ret
.Lfe9:
	.size	 NodeInsert,.Lfe9-NodeInsert
	.align 4
.globl NodeRemove
	.type	 NodeRemove,@function
NodeRemove:
	movl 4(%esp),%eax
	movl (%eax),%edx
	movl 4(%eax),%eax
	movl %eax,4(%edx)
	movl %edx,(%eax)
	ret
.Lfe10:
	.size	 NodeRemove,.Lfe10-NodeRemove
	.align 4
.globl NodeNext
	.type	 NodeNext,@function
NodeNext:
	pushl %ebx
	movl 8(%esp),%ebx
	pushl %ebx
	call NodeValid
	addl $4,%esp
	testl %eax,%eax
	je .L14
	movl (%ebx),%ebx
.L14:
	movl %ebx,%eax
	popl %ebx
	ret
.Lfe11:
	.size	 NodeNext,.Lfe11-NodeNext
	.align 4
.globl NodePrev
	.type	 NodePrev,@function
NodePrev:
	pushl %ebx
	movl 8(%esp),%ebx
	pushl %ebx
	call NodeValid
	addl $4,%esp
	testl %eax,%eax
	je .L16
	movl 4(%ebx),%ebx
.L16:
	movl %ebx,%eax
	popl %ebx
	ret
.Lfe12:
	.size	 NodePrev,.Lfe12-NodePrev
	.align 4
.globl NodeNextW
	.type	 NodeNextW,@function
NodeNextW:
	pushl %ebx
	movl 8(%esp),%ebx
	pushl %ebx
	call NodeValid
	addl $4,%esp
	testl %eax,%eax
	je .L18
	movl (%ebx),%ebx
	pushl %ebx
	call NodeValid
	addl $4,%esp
	testl %eax,%eax
	jne .L18
	movl -4(%ebx),%ebx
.L18:
	movl %ebx,%eax
	popl %ebx
	ret
.Lfe13:
	.size	 NodeNextW,.Lfe13-NodeNextW
	.align 4
.globl NodePrevW
	.type	 NodePrevW,@function
NodePrevW:
	pushl %ebx
	movl 8(%esp),%ebx
	pushl %ebx
	call NodeValid
	addl $4,%esp
	testl %eax,%eax
	je .L21
	movl 4(%ebx),%ebx
	pushl %ebx
	call NodeValid
	addl $4,%esp
	testl %eax,%eax
	jne .L21
	movl 8(%ebx),%ebx
.L21:
	movl %ebx,%eax
	popl %ebx
	ret
.Lfe14:
	.size	 NodePrevW,.Lfe14-NodePrevW
	.align 4
.globl NodeValid
	.type	 NodeValid,@function
NodeValid:
	movl 4(%esp),%eax
	cmpl $0,(%eax)
	jne .L24
	xorl %eax,%eax
	ret
	.align 4
.L24:
	cmpl $0,4(%eax)
	je .L25
	movl $1,%eax
	ret
	.align 4
.L25:
	xorl %eax,%eax
	ret
.Lfe15:
	.size	 NodeValid,.Lfe15-NodeValid
	.ident	"GCC: (GNU) 2.7.2.3"
