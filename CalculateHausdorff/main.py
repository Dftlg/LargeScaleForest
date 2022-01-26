import numpy as np
import operator


def Hausdorff():
    dataTarget = np.loadtxt("D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/OthrVegType/70/Hausdorff433/voxelnumber35/EachPartSkelCubic_index_hausdorffobj/99.txt")
    dataTruth = np.loadtxt("D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/OthrVegType/70/Hausdorff433/voxelnumber35/groundTruth/positionthe0phi60force1Index19_99.txt")
    # A = [[1, 2, 3],
    #      [4, 5, 6]]
    # B = [[1, 2, 43],
    #      [4, 5, 6]]

    # print(dataTarget)
    # print(dataTruth)
    asize = [len(dataTarget), len(dataTarget[0])]
    bsize = [len(dataTruth), len(dataTruth[0])]


    if asize[1] != bsize[1]:
        print('The dimensions of points in the two sets are not equal')

    fhd = 0
    for i in range(asize[0]):
        mindist = float("inf")
        for j in range(bsize[0]):
            tempdist = np.linalg.norm(list(map(operator.sub, dataTarget[i], dataTruth[j])))
            if tempdist < mindist:
                mindist = tempdist
        fhd = fhd + mindist
    fhd = fhd / asize[0]

    rhd = 0
    for j in range(bsize[0]):
        mindist = float("inf")
        for i in range(asize[0]):
            tempdist = np.linalg.norm(list(map(operator.sub, dataTarget[i], dataTruth[j])))
            if tempdist < mindist:
                mindist = tempdist
        rhd = rhd + mindist
    rhd = rhd / bsize[0]

    mhd = max(fhd, rhd)

    # print(fhd)
    # print(rhd)
    print(mhd)


# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    Hausdorff()

# See PyCharm help at https://www.jetbrains.com/help/pycharm/
