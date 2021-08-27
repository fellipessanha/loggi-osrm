import matplotlib.pyplot as plt
from sklearn import cluster
import json

def readData(datapath):
  with open(datapath, 'r') as f:
    jsonfile = json.load(f)
    f.close()

  route_dict = dict()
  for i in jsonfile['deliveries']:
    rtId = i['id']
    lng = i['point']['lng']
    lat = i['point']['lat']
    #
    route_dict[rtId] = [lng, lat]

  return route_dict

def KMeans_pred(n_clust, routes):
  
  coordinates = [i for i in routes.values()]
  rtIds = [i for i in routes.keys()]

  y_pred = cluster.KMeans(n_clusters=n_clust, random_state=100).fit_predict(coordinates)

  plt.scatter([i[0] for i in coordinates], [i[1] for i in coordinates], c=y_pred, s=3)
  plt.title(f"kmeans with {n_clust} clusters")
  plt.savefig("clusterings/kmeans.jpg")
  # plt.show()

  labels_per_id = {}
  for i in range(len(routes)):
    labels_per_id[rtIds[i]] = int(y_pred[i])

  return labels_per_id

def completeAgglomerative_pred(n_clust, routes):

  coordinates = [i for i in routes.values()]
  rtIds = [i for i in routes.keys()]

  y_pred = cluster.AgglomerativeClustering(n_clusters= n_clust, linkage='complete').fit(coordinates)
  labels = y_pred.labels_
  plt.scatter([i[0] for i in coordinates], [i[1] for i in coordinates], c=labels, s=3)
  plt.title(f"complete agglomerative with {n_clust} clusters")
  plt.savefig("clusterings/completeAgglomerative.jpg")
  
  labels_per_id = {}
  for i in range(len(routes)):
    labels_per_id[rtIds[i]] = int(labels[i])

  return labels_per_id

def wardAgglomerative_pred(n_clust, routes):

  coordinates = [i for i in routes.values()]
  rtIds = [i for i in routes.keys()]

  y_pred = cluster.AgglomerativeClustering(n_clusters=n_clust, linkage='ward').fit(coordinates)
  labels = y_pred.labels_
  plt.scatter([i[0] for i in coordinates], [i[1] for i in coordinates], c=labels, s=3)
  plt.title(f"ward agglomerative with {n_clust} clusters")
  plt.savefig("clusterings/wardAgglomerative.jpg")

  
  labels_per_id = {}
  for i in range(len(routes)):
    labels_per_id[rtIds[i]] = int(labels[i])

  return labels_per_id


def dbScan_pred(min_dist, min_neighbors, routes):

  coordinates = [i for i in routes.values()]
  rtIds = [i for i in routes.keys()]

  y_pred = cluster.DBSCAN(min_samples=min_neighbors, eps=min_dist).fit(coordinates)
  labels = y_pred.labels_
  plt.scatter([i[0] for i in coordinates], [i[1] for i in coordinates], c=labels, s=3)
  plt.title(f"DBScan: esp = {min_dist}, min_samples = {min_neighbors}")
  plt.savefig("clusterings/DBSCAN.jpg")

  labels_per_id = {}
  for i in range(len(routes)):
    labels_per_id[rtIds[i]] = int(labels[i])

  return labels_per_id

def optics_pred(min_dist, min_neighbors, coordinates):
  y_pred = cluster.OPTICS(min_samples=50, xi=.05, min_cluster_size=.05).fit(coordinates)
  labels = y_pred.labels_
  plt.scatter([i[0] for i in coordinates], [i[1] for i in coordinates], c=labels, s=3)
  plt.title(f"DBScan: esp = {min_dist}, min_samples = {min_neighbors}")
  plt.savefig("clusterings/DBSCAN.jpg")

  return labels

def main():
  routeDict = readData('data/cvrp-0-rj-0.json')

  # Used to 
  sz = len(routeDict)
  n_clusters = int(sz/150)
  min_dist = 0.0004
  min_neighbors = 20

  output = KMeans_pred(n_clusters, routeDict)
  with open('clusterings/kmeans-clusteringLabels.json', 'w') as f:
    json.dump(output, f)

  output =  wardAgglomerative_pred(n_clusters, routeDict)
  with open('clusterings/wardAgllomerative-clusteringLabels.json', 'w') as f:
    json.dump(output, f)
  
  output = completeAgglomerative_pred(n_clusters, routeDict),
  with open('clusterings/complete_agglomerative-clusteringLabels.json', 'w') as f:
    json.dump(output, f)

  output =  dbScan_pred(min_dist, min_neighbors, routeDict)
  with open('clusterings/dbScan-clusteringLabels.json', 'w') as f:
    json.dump(output, f)


if __name__ == "__main__":
    main()