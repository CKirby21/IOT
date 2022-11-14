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

const containerStyle = {
  width: '1000px',
  height: '600px'
};

// TODO add current location
const center = {
  lat: 40.802,
  lng: -96.734
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

const minRadius = 100;
const maxRadius = 1000;

const MapWrapper: React.FC<{}> = () => {
  

  // Geo Fencing
  const [isValidRadius, setIsValidRadius] = useState<boolean>();
  const [radius, setRadius] = useState(minRadius);
  const [savedRadius, setSavedRadius] = useState<number>(100);

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
                mapContainerStyle={containerStyle}
                center={center}
                zoom={16}
              >
                <MarkerF
                  // icon={"https://developers.google.com/maps/documentation/javascript/examples/full/images/beachflag.png"}
                  position={center}
                />
                <CircleF center={center} radius={savedRadius} options={circleOptions}/>
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