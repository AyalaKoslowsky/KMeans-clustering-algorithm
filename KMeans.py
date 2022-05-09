import sys
import math


# reads data points from filename into a list
def read_data_points_from_file(filename):
    try:
        file = open(filename, 'r')
    except:
        print("An Error Has Occurred")
        sys.exit()

    data_points_from_file = []
    for line in file:
        vector = line.split(',')
        vector = [float(i) for i in vector]
        data_points_from_file.append(vector)
    file.close()
    return data_points_from_file


def kmeans(k, max_iter, data_points, epsilon):
    # k first vectors -> the initial k centroids , and puts them into the k clusters
    centroids = data_points[:k]
    clusters = [[[]]]
    for vector in centroids:
        clusters.append([vector])
    clusters.remove(clusters[0])

    iterations = 0
    while iterations < max_iter:
        iterations += 1
        points_closest_center = get_all_distances(centroids, data_points, k)
        clusters = create_clusters(clusters, points_closest_center, data_points, k)
        new_centroids = []
        for i in range(k):
            new_centroids.append(find_new_center(clusters[i]))
        delta = 0
        for j in range(k):
            j_dist = float(distance(centroids[j], new_centroids[j]))
            if j_dist > delta:
                delta = j_dist
        if delta < epsilon:
            centroids = new_centroids
            break
        centroids = new_centroids
    for centroid in new_centroids:
        for i in range(len(centroid)):
            centroid[i] = float("{0:.4f}".format(centroid[i]))
    return centroids


def get_all_distances(centroids, data_points, k):
    points_distances = [-1] * len(data_points)
    for i in range(len(data_points)):
        point = data_points[i]
        min_dist = sys.maxsize
        closest_index = 0
        for j in range(k):
            dist = distance(point, centroids[j])
            if dist < min_dist:
                min_dist = dist
                closest_index = j
        points_distances[i] = closest_index
    return points_distances


def distance(first, second):
    curr_sum = 0
    for i in range(len(first)):
        curr_sum += (first[i] - second[i]) ** 2
    # return float("{0:.4f}".format(math.sqrt(curr_sum)))
    return math.sqrt(curr_sum)


def create_clusters(clusters, points_closest_center, data_points, k):
    for i in range(len(clusters)):
        clusters[i] = []
    for i in range(len(data_points)):
        clusters[points_closest_center[i]].append(data_points[i])
    return clusters


def find_new_center(cluster_vectors):
    cluster_center = []
    for i in range(len(cluster_vectors[0])):
        curr_sum = 0
        for vector in cluster_vectors:
            curr_sum += vector[i]
        if len(cluster_vectors) == 0:
            print("An Error Has Occurred")
            sys.exit()
        cluster_center.append(curr_sum / len(cluster_vectors))
    return cluster_center


def write_data_points_to_output_file(centroids, output_file_name):
    try:
        output_file = open(output_file_name, 'w')
    except:
        print("An Error Has Occurred")
        sys.exit()
    for vector in centroids:
        for i in range(len(vector)):
            x = "{0:.4f}".format(float(vector[i]))
            if i != (len(vector) - 1):
                output_file.write(x + ',')
            else:
                output_file.write(x + '\n')
    output_file.close()
    return output_file


def main(args):
    # initialize args from user

    # check num of args
    if len(args) > 5 or len(args) < 4:
        print("Invalid Input!")
        return

    # check k is a number
    k = args[1]
    try:
        k = int(args[1])
    except:
        print("Invalid Input!")
        return

    # check k > 1
    if k < 1:
        print("Invalid Input!")
        return

    max_iter = 200  # default

    if len(args) == 5:
        try:
            max_iter = int(args[2])
        except:
            print("Invalid Input!")
            return
        input_file_name = args[3]
        output_file_name = args[4]
    else:
        max_iter = 200
        input_file_name = args[2]
        output_file_name = args[3]

    # check num of args
    if max_iter < 1:
        print("Invalid Input!")
        return

    # check files names
    if (input_file_name[-4:] != ".txt") or (output_file_name[-4:] != ".txt"):
        print("Invalid Input!")
        return

    # default
    epsilon = 0.001

    # reads input file into data points list
    data_points = read_data_points_from_file(input_file_name)

    # check k<N
    if k >= len(data_points):
        print("Invalid Input!")
        return

    # k-means on data points list
    # returns centroids
    centroids = kmeans(k, max_iter, data_points, epsilon)

    # write final centroids into the output file
    output_file = write_data_points_to_output_file(centroids, output_file_name)
    return


# MAIN
main(sys.argv)
