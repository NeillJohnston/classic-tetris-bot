"""# Inputs the piece table and outputs Lua tables.
n = int(input())
out = ''

for i in range(n):
    line = input().split()
    xs = [line[i] for i in range(0, 8, 2)]
    ys = [line[i] for i in range(1, 8, 2)]
    xys = zip(xs, ys)
    curly = lambda x: '{' + x + '}'
    out += curly(', '.join(curly(f'x={_[0]}, y={_[1]}') for _ in xys)) + '\n'

print(out)
"""

# Inputs piece rotations and rotation codes (the next piece on a positive rotation and how to get there), outputs Lua tables.
n = int(input())
out = ''

for i in range(n):
    r, c = input().split()
    out += '\t{' + f'' + '}\n'
