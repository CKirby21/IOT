import {
  IonContent,
  IonHeader,
  IonItem,
  IonPage,
  IonTitle,
  IonToolbar,
  IonButton,
} from "@ionic/react";
import React, { useState, useRef, useEffect } from "react";
import Header from "../../components/header/header";
import Map from "ol/Map";
import View from "ol/View";
import TileLayer from "ol/layer/Tile";
import OSM from "ol/source/OSM";

const map = new Map({
  view: new View({
    center: [0, 0],
    zoom: 2,
  }),
  layers: [
    new TileLayer({
      source: new OSM(),
    }),
  ],
  target: "map",
});

const view = map.getView();
const zoom = view.getZoom();

const MapWrapper: React.FC = () => {
  return (
    <IonPage>
      <IonHeader>
        <IonToolbar>
          <IonTitle>Map</IonTitle>
        </IonToolbar>
      </IonHeader>
      <IonContent>
        <Header />
        <IonItem className="zoom-out">
          <IonButton onClick={() => view.setZoom(zoom == null ? 0 : zoom - 1)}>
            Zoom Out
          </IonButton>
        </IonItem>

        <IonItem className="zoom-in">
          <IonButton onClick={() => view.setZoom(zoom == null ? 0 : zoom + 1)}>
            Zoom In
          </IonButton>
        </IonItem>
      </IonContent>
    </IonPage>
  );
};

export default MapWrapper;
