import subprocess
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

test_files = ['test64', 'test128', 'test256', 'test512', 'test1024', 'test2048', 'test4096', 'test8192']
test_out = {}
for file in test_files:
	test_out[file] = {}
	for w in range(4, 15):
		for l in range(3, w):
			p = subprocess.Popen(['./heatshrink', '-e', '-v', '-w', str(w), \
				 '-l', str(l), file, 'output'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
			out, err = p.communicate()
			test_out[file][(w, l)] = float(out.split()[1]) / 100

fig = plt.figure()
ax = Axes3D(fig)
coords_x, coords_y = [], []
zs = []
for pts, num in test_out['test8192'].items():
	coords_x.append(pts[0])
	coords_y.append(pts[1])
	zs.append(num)

# ax.scatter(coords_x, coords_y, zs)
ax.plot_trisurf(coords_x, coords_y, zs, cmap='jet')
ax.grid(True)
ax.set_xlabel('window size')
ax.set_ylabel('back reference len')
ax.set_zlabel('compression rate')
plt.savefig('test8192_surf_param.jpg')