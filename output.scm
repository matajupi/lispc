(
define
(
front-ptr
queue
)
(
car
queue
)
)
(
define
(
rear-ptr
queue
)
(
cdr
queue
)
)
(
define
(
set-front-ptr!
queue
item
)
(
set-car!
queue
item
)
)
(
define
(
set-rear-ptr!
queue
item
)
(
set-cdr!
queue
item
)
)
(
define
(
make-queue
)
(
cons
'
(
)
'
(
)
)
)
(
define
(
empty-queue?
queue
)
(
null?
(
front-ptr
queue
)
)
)
(
define
(
front-queue
queue
)
(
if
(
empty-queue?
queue
)
(
error
"
FRONT
called
with
an
empty
queue
"
queue
)
(
car
(
front-ptr
queue
)
)
)
)
(
define
(
insert-queue!
queue
item
)
(
let
(
(
new-pair
(
cons
item
'
(
)
)
)
)
(
cond
(
(
empty-queue?
queue
)
(
set-front-ptr!
queue
new-pair
)
(
set-rear-ptr!
queue
new-pair
)
queue
)
(
else
(
set-cdr!
(
rear-ptr
queue
)
new-pair
)
(
set-rear-ptr!
queue
new-pair
)
queue
)
)
)
)
(
define
(
delete-queue!
queue
)
(
cond
(
(
empty-queue?
queue
)
(
error
"
DELETE!
called
with
an
empty
queue
"
queue
)
)
(
else
(
set-front-ptr!
queue
(
cdr
(
front-ptr
queue
)
)
)
queue
)
)
)
(
define
(
make-time-segment
time
queue
)
(
cons
time
queue
)
)
(
define
(
segment-time
s
)
(
car
s
)
)
(
define
(
segment-queue
s
)
(
cdr
s
)
)
(
define
(
make-agenda
)
(
list
140727296496176
)
)
(
define
(
current-time
agenda
)
(
car
agenda
)
)
(
define
(
set-current-time!
agenda
time
)
(
set-car!
agenda
time
)
)
(
define
(
segments
agenda
)
(
cdr
agenda
)
)
(
define
(
set-segments!
agenda
segments
)
(
set-cdr!
agenda
segments
)
)
(
define
(
first-segment
agenda
)
(
car
(
segments
agenda
)
)
)
(
define
(
rest-segments
agenda
)
(
cdr
(
segments
agenda
)
)
)
(
define
(
empty-agenda?
agenda
)
(
null?
(
segments
agenda
)
)
)
(
define
(
add-to-agenda!
time
action
agenda
)
(
define
(
belongs-before?
segments
)
(
or
(
null?
segments
)
(
<
time
(
segment-time
(
car
segments
)
)
)
)
)
(
define
(
make-new-time-segment
time
action
)
(
let
(
(
q
(
make-queue
)
)
)
(
insert-queue!
q
action
)
(
make-time-segment
time
q
)
)
)
(
define
(
add-to-segments!
segments
)
(
if
(
=
(
segment-time
(
car
segments
)
)
time
)
(
insert-queue!
(
segment-queue
(
car
segments
)
)
action
)
(
let
(
(
rest
(
cdr
segments
)
)
)
(
if
(
belongs-before?
rest
)
(
set-cdr!
segments
(
cons
(
make-new-time-segment
time
action
)
(
cdr
segments
)
)
)
(
add-to-segments!
rest
)
)
)
)
)
(
let
(
(
segments
(
segments
agenda
)
)
)
(
if
(
belongs-before?
segments
)
(
set-segments!
agenda
(
cons
(
make-new-time-segment
time
action
)
segments
)
)
(
add-to-segments!
segments
)
)
)
)
(
define
(
remove-first-agenda-item!
agenda
)
(
let
(
(
q
(
segment-queue
(
first-segment
agenda
)
)
)
)
(
delete-queue!
q
)
(
if
(
empty-queue?
q
)
(
set-segments!
agenda
(
rest-segments
agenda
)
)
)
)
)
(
define
(
first-agenda-item
agenda
)
(
if
(
empty-agenda?
agenda
)
(
error
"
Agenda
is
empty:
FIRST-AGENDA-ITEM
"
)
(
let
(
(
first-seg
(
first-segment
agenda
)
)
)
(
set-current-time!
agenda
(
segment-time
first-seg
)
)
(
front-queue
(
segment-queue
first-seg
)
)
)
)
)
(
define
(
after-delay
delay
action
)
(
add-to-agenda!
(
+
delay
(
current-time
the-agenda
)
)
action
the-agenda
)
)
(
define
(
propagate
)
(
if
(
empty-agenda?
the-agenda
)
'
done
(
let
(
(
first-time
(
first-agenda-item
the-agenda
)
)
)
(
first-time
)
(
remove-first-agenda-item!
the-agenda
)
(
propagate
)
)
)
)
(
define
(
probe
name
wire
)
(
add-action!
wire
(
lambda
(
)
(
display
name
)
(
display
"
"
)
(
display
(
current-time
the-agenda
)
)
(
display
"
New-value
=
"
)
(
display
(
get-signal
wire
)
)
(
newline
)
)
)
)
(
define
(
make-wire
)
(
let
(
(
signal-value
140727296496176
)
(
action-procedures
'
(
)
)
)
(
define
(
set-my-signal!
new-value
)
(
if
(
not
(
=
signal-value
new-value
)
)
(
begin
(
set!
signal-value
new-value
)
(
call-each
action-procedures
)
)
'
done
)
)
(
define
(
accept-action-procedure!
proc
)
(
set!
action-procedures
(
cons
proc
action-procedures
)
)
(
proc
)
)
(
define
(
dispatch
m
)
(
cond
(
(
eq?
m
'
get-signal
)
signal-value
)
(
(
eq?
m
'
set-signal!
)
set-my-signal!
)
(
(
eq?
m
'
add-action!
)
accept-action-procedure!
)
(
else
(
error
"
Unknown
operation:
WIRE
"
m
)
)
)
)
dispatch
)
)
(
define
(
call-each
procedures
)
(
if
(
null?
procedures
)
'
done
(
begin
(
(
car
procedures
)
)
(
call-each
(
cdr
procedures
)
)
)
)
)
(
define
(
get-signal
wire
)
(
wire
'
get-signal
)
)
(
define
(
set-signal!
wire
new-value
)
(
(
wire
'
set-signal!
)
new-value
)
)
(
define
(
add-action!
wire
action-procedure
)
(
(
wire
'
add-action!
)
action-procedure
)
)
(
define
(
inverter
input
output
)
(
define
(
invert-input
)
(
let
(
(
new-value
(
logical-not
(
get-signal
input
)
)
)
)
(
after-delay
inverter-delay
(
lambda
(
)
(
set-signal!
output
new-value
)
)
)
)
)
(
add-action!
input
invert-input
)
'
ok
)
(
define
(
logical-not
s
)
(
cond
(
(
=
s
140727296496176
)
140727296496177
)
(
(
=
s
140727296496177
)
140727296496176
)
(
else
(
error
"
Invalid
signal
"
)
)
)
)
(
define
(
and-gate
a1
a2
output
)
(
define
(
and-action-procedure
)
(
let
(
(
new-value
(
logical-and
(
get-signal
a1
)
(
get-signal
a2
)
)
)
)
(
after-delay
and-gate-delay
(
lambda
(
)
(
set-signal!
output
new-value
)
)
)
)
)
(
add-action!
a1
and-action-procedure
)
(
add-action!
a2
and-action-procedure
)
'
ok
)
(
define
(
logical-and
a1
a2
)
(
cond
(
(
or
(
=
a1
140727296496176
)
(
=
a2
140727296496176
)
)
140727296496176
)
(
(
and
(
=
a1
140727296496177
)
(
=
a2
140727296496177
)
)
140727296496177
)
(
else
(
error
"
Invalid
signal
"
)
)
)
)
(
define
(
or-gate
a1
a2
output
)
(
define
(
or-action-procedure
)
(
let
(
(
new-value
(
logical-or
(
get-signal
a1
)
(
get-signal
a2
)
)
)
)
(
after-delay
or-gate-delay
(
lambda
(
)
(
set-signal!
output
new-value
)
)
)
)
)
(
add-action!
a1
or-action-procedure
)
(
add-action!
a2
or-action-procedure
)
'
ok
)
(
define
(
logical-or
a1
a2
)
(
cond
(
(
and
(
=
a1
140727296496176
)
(
=
a2
140727296496176
)
)
140727296496176
)
(
(
or
(
=
a1
140727296496177
)
(
=
a2
140727296496177
)
)
140727296496177
)
(
else
(
error
"
Invalid
signal
"
)
)
)
)
(
define
(
half-adder
a
b
s
c
)
(
let
(
(
d
(
make-wire
)
)
(
e
(
make-wire
)
)
)
(
or-gate
a
b
d
)
(
and-gate
a
b
c
)
(
inverter
c
e
)
(
and-gate
d
e
s
)
'
ok
)
)
(
define
(
full-adder
a
b
c-in
sum
c-out
)
(
let
(
(
s
(
make-wire
)
)
(
c1
(
make-wire
)
)
(
c2
(
make-wire
)
)
)
(
half-adder
b
c-in
s
c1
)
(
half-adder
a
s
sum
c2
)
(
or-gate
c1
c2
c-out
)
'
ok
)
)
(
define
the-agenda
(
make-agenda
)
)
(
define
inverter-delay
140727296496177
)
(
define
and-gate-delay
140727296496177
)
(
define
or-gate-delay
140727296496177
)
(
define
in1
(
make-wire
)
)
(
define
in2
(
make-wire
)
)
(
define
sum
(
make-wire
)
)
(
define
carry
(
make-wire
)
)
(
probe
'
sum
sum
)
(
probe
'
carry
carry
)
(
set-signal!
in2
140727296496177
)
(
half-adder
in1
in2
sum
carry
)
(
propagate
)
(
print
(
get-signal
sum
)
)
(
print
(
get-signal
carry
)
)
(
define
(
or-gate
a1
a2
output
)
(
let
(
(
c
(
make-wire
)
)
(
d
(
make-wire
)
)
(
e
(
make-wire
)
)
)
(
inverter
a1
c
)
(
inverter
a2
d
)
(
and-gate
c
d
e
)
(
inverter
e
output
)
'
ok
)
)
(
define
in1
(
make-wire
)
)
(
define
in2
(
make-wire
)
)
(
define
output
(
make-wire
)
)
(
set-signal!
in1
140727296496177
)
(
or-gate
in1
in2
output
)
(
propagate
)
(
print
(
get-signal
output
)
)
