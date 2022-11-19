import {
  IonContent,
  IonHeader,
  IonItem,
  IonLabel,
  IonPage,
  IonTitle,
  IonToolbar,
  IonInput,
  IonButton,
  IonNote,
  IonGrid,
  IonRow,
  IonCol,
  IonText,
} from "@ionic/react";
import React, { useState } from "react";
import Header from "../../components/header/header";
import { CircleF, GoogleMap, LoadScript, MarkerF} from '@react-google-maps/api';

const mapContainerStyle = {
  width: '1100px',
  height: '600px'
};

const circleOptions = {
  strokeColor: '#FF0000',
  strokeOpacity: 0.8,
  strokeWeight: 2,
  fillColor: '#FF0000',
  fillOpacity: 0.35,
  clickable: false,
  draggable: false,
  editable: false,
  visible: true
}

const MapWrapper: React.FC<{}> = () => {
  
  const minRadius = 20;
  const maxRadius = 1000;
  const [isValidRadius, setIsValidRadius] = useState<boolean>();
  const [radius, setRadius] = useState(minRadius);
  const [savedRadius, setSavedRadius] = useState<number>(minRadius);
  const [center, setCenter] = useState({lat: 0, lng: 0});
  const [savedCenter, setSavedCenter] = useState({lat: 40.8207, lng: -96.7056});

  /* Functions for Radius */
  const validateRadius = (ev: Event) => {
    const input = (ev.target as HTMLInputElement).value;
    setIsValidRadius(undefined);
    if (input === "") return;

    const inputRadius = parseInt(input);
    setRadius(inputRadius);
    if (minRadius <= inputRadius && inputRadius <= maxRadius) {
      setIsValidRadius(true);
    } else {
      setIsValidRadius(false);
    }
  };

  const saveRadius = () => {
    if (isValidRadius) setSavedRadius(radius);
  };
  
  /* Functions for Center */
  function getCenterSuccess(position: { coords: any; }) {
    setCenter({lat: position.coords.latitude, lng: position.coords.longitude});
  }
  
  function getCenterError(err: { code: any; message: any; }) {
    console.warn(`ERROR(${err.code}): ${err.message}`);
  }
  
  const getCenter = () => {
    const options = {
      enableHighAccuracy: true,
      timeout: 5000,
      maximumAge: 0
    };
    navigator.geolocation.getCurrentPosition(getCenterSuccess, getCenterError, options);
  };

  const saveCenter = () => {
    if (center.lat !== 0 && center.lng !== 0) setSavedCenter(center);
  };

  const formatCenter = (centerToFormat: any) => {
    if (centerToFormat.lat === 0 && centerToFormat.lng === 0) 
      return "";
    return centerToFormat.lat.toFixed(3) + ", " + centerToFormat.lng.toFixed(3);
  };


  return (
    <IonPage>
      <IonHeader>
        <IonToolbar>
          <IonTitle>Map</IonTitle>
        </IonToolbar>
      </IonHeader>
      <IonContent>
        <Header />
        <IonGrid>
          <IonRow>


            <LoadScript
            googleMapsApiKey="AIzaSyBYrl_wYHZ1QBg_0CNqu7XOKbzi9sIjJ2E"
            >
              <GoogleMap
                mapContainerStyle={mapContainerStyle}
                center={savedCenter}
                zoom={17}
              >
                <MarkerF
                  // icon={"https://developers.google.com/maps/documentation/javascript/examples/full/images/beachflag.png"}
                  position={savedCenter}
                />
                <CircleF center={savedCenter} radius={savedRadius} options={circleOptions}/>
              </GoogleMap>
            </LoadScript>


            <IonGrid fixed={true}>
              <IonRow>
                <IonCol>

                  <IonItem>
                    <IonText>
                      <h1>Fence Settings</h1>
                    </IonText>
                  </IonItem>

                  <IonItem/>

                  <IonItem>
                    <IonLabel>Saved Center:</IonLabel>
                    <IonInput
                      value={formatCenter(savedCenter)}
                      disabled={true}
                    ></IonInput>
                  </IonItem>

                  <IonItem>
                    <IonLabel>{"New Center: " + formatCenter(center)}</IonLabel>
                    <IonButton onClick={() => getCenter()}>Fetch</IonButton>
                  </IonItem>

                  <IonItem>
                    <IonButton onClick={() => saveCenter()}>Save</IonButton>
                  </IonItem>

                  <IonItem/>

                  <IonItem>
                    <IonLabel>Saved Radius:</IonLabel>
                    <IonInput
                      value={savedRadius + " meters"}
                      disabled={true}
                    ></IonInput>
                  </IonItem>

                  <IonItem
                    className={`${isValidRadius && "ion-valid"} ${
                      isValidRadius === false && "ion-invalid"
                    }`}>
                    <IonLabel>New Radius:</IonLabel>
                    <IonInput
                      type="number"
                      defaultValue={minRadius}
                      onIonInput={(event) => validateRadius(event)}
                    ></IonInput>
                    <IonNote slot="error">
                      Radius must be a number between {minRadius} and {maxRadius} meters
                    </IonNote>
                  </IonItem>

                  <IonItem>
                    <IonButton onClick={() => saveRadius()}>Save</IonButton>
                  </IonItem>

                </IonCol>
              </IonRow>
            </IonGrid>


          </IonRow>
        </IonGrid>
      </IonContent>
    </IonPage>
  );
};

export default MapWrapper;