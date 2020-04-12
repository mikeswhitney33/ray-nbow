from distribution_functions import EpsilonGreedy
from rendering import render_scene
import numpy as np
import cv2 as cv


def main():
    eps = EpsilonGreedy()
    for _ in range(5):
        image = render_scene("scenes/basic.json", eps, 3, (224, 224))
        cvimage = np.array(image)
        cv.imshow("cvimage", cvimage)
        score = 0
        if cv.waitKey(0) & 0xff == ord('y'):
            score = 1
        eps.update(score)
    eps.print_seed_lengths()
    cv.destroyAllWindows()

if __name__ == "__main__":
    main()
