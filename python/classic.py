import cv2 as cv 
import numpy as np 
import queue

def check_neighbor(y, x, visited, lowest, dist):
    h, w = visited.shape
    return y < h and y >= 0 and x < w and x >= 0 and visited[y, x] == 0 and dist[y,x] < lowest and dist[y,x] != 0

def main():
    im = cv.imread("../build/resources/depth/bunny.png", cv.IMREAD_COLOR)
    dist = cv.cvtColor(im, cv.COLOR_BGR2GRAY)
    fdist = dist.astype(np.float32) / 255

    fim = im.astype(np.float32)/255
    visited = np.zeros_like(fim[:,:,0])
    h, w = fim.shape[:2]
    c = np.random.uniform(0, 1, (3))
    for _ in range(1000):
        
        while True:
            y = np.random.randint(0, fim.shape[1]-1)
            x = np.random.randint(0, fim.shape[0]-1)
            if np.sum(fim[x, y]) != 0:
                break
        q = queue.Queue()
        q.put((y, x))
        while not q.empty():
            c = np.clip(c + np.random.uniform(-0.05, 0.05, size=(3,)), 0, 1)
            p = q.get()
            
            y, x = p 
            if visited[y,x] == 0:
                cv.circle(fim, (x, y), 1, c * fdist[y,x], -1)
            visited[y,x] = 1
            lowest = 255*3
            bssf = None
            if check_neighbor(y + 1, x, visited, lowest, dist):
                bssf = (y+1,x)
                lowest = dist[y,x]
            if check_neighbor(y, x + 1, visited, lowest, dist):
                bssf = (y,x+1)
                lowest = dist[y,x]
            if check_neighbor(y - 1, x, visited, lowest, dist):
                bssf = (y-1,x)
                lowest = dist[y,x]
            if check_neighbor(y, x - 1, visited, lowest, dist):
                bssf = (y,x-1)
                lowest = dist[y,x]
            if bssf:
                q.put(bssf)
    cv.imshow("bunny", fim)
    cv.waitKey()
    cv.destroyAllWindows()


if __name__ == '__main__':
    main()