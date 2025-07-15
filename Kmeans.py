import sys
import math

class incorrectnumberofclusters(Exception):
    pass
class incorrectnumberofiterations(Exception):
    pass    

def euclidean_distance(p1, p2):
    return math.sqrt(sum((x - y) ** 2 for x, y in zip(p1, p2)))

def Kmeans(K, points, iter=400):
    """
    K-means clustering algorithm.
    Parameters: 
    k (int): Number of clusters.
    iter (int): Number of iterations.
    input_file (str): Path to the input file.

    Returns:
    centroids (list): List of centroids for each cluster.

    """    
    if K <= 1:
        raise incorrectnumberofclusters("Number of clusters must be greater than 0.")
    if K >= len(points):
        raise incorrectnumberofclusters("Number of clusters must be less than the number of points.")   
    if iter <= 1:   
        raise incorrectnumberofiterations("Number of iterations must be greater than 1.")
    if iter >= 1000:
        raise incorrectnumberofiterations("Number of iterations must be less than 1000.")
    
    epsilon = 1e-3
    
    
    centroids = [list(point) for point in points[:K]]
    prev_centroids = [list(point) for point in centroids]

    # Initialize iteration counter  
    iteration = 0
    # Loop until convergence or max iterations
    while iteration < iter: 
        
        clusters = [[] for _ in range(K)]

        # Assign points to the nearest centroid
        for point in points:
            distances = [euclidean_distance(point, centroid) for centroid in centroids]
            cluster_index = distances.index(min(distances))
            clusters[cluster_index].append(point)
        
        # Update centroids
        for i in range(K):
            if len(clusters[i]) > 0:
                centroids[i] = [sum(coord) / len(clusters[i]) for coord in zip(*clusters[i])]
        
        # Check for convergence
        if all(euclidean_distance(centroids[i], prev_centroids[i]) < epsilon for i in range(K)):
            break
        
        # Update previous centroids
        prev_centroids = [list(centroid) for centroid in centroids]
        
        iteration += 1  

    return centroids

def main():
    try:
        if len(sys.argv) not in [2,3]:
            print("Invalid number of arguments!")
            return

        K = int(sys.argv[1])
        if len(sys.argv) == 3:
            if not sys.argv[2].isdigit():
                raise incorrectnumberofiterations("Maximum iterations must be a positive integer.")
            else:
                max_iter = int(sys.argv[2])
        else:
            max_iter = 400

        # Read from stdin
        points = []
        for line in sys.stdin:
            if line.strip():
                coords = tuple(map(float, line.strip().split(',')))
                points.append(coords)

        centroids = Kmeans(K, points, max_iter)

        for centroid in centroids:
            print(','.join(f"{coord:.4f}" for coord in centroid))

    except incorrectnumberofclusters:
        print("Invalid number of clusters!")
    except incorrectnumberofiterations:
        print("Invalid maximum iteration!")
    except Exception:
        print("An Error Has Occurred")

if __name__ == "__main__":
    main()
        

    
        
    
