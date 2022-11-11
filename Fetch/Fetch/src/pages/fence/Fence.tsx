import { IonContent, IonHeader, IonItem, IonLabel, IonPage, IonTitle, IonToolbar, IonInput, IonButton, IonNote } from '@ionic/react';
import { useState } from 'react';
import './Fence.css';
import Header from "../../components/header/header";

const minRadius = 100;
const maxRadius = 1000;

const Fence: React.FC = () => {

  const [isValidRadius, setIsValidRadius] = useState<boolean>();
  const [radius, setRadius] = useState(minRadius);
  const [savedRadius, setSavedRadius] = useState<number>(); 

  const validateRadius = (ev: Event) => {

    const input = (ev.target as HTMLInputElement).value;
    setIsValidRadius(undefined);
    if (input === '') return;
    
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
          <IonTitle>Home</IonTitle>
        </IonToolbar>
      </IonHeader>

      <IonContent>
          <Header />
          <IonItem className="current-radius">
            <IonLabel>Saved Radius:</IonLabel>
            <IonInput type="number" value={savedRadius} disabled={true}></IonInput>
          </IonItem>

          <IonItem className={`${isValidRadius && 'ion-valid'} ${isValidRadius === false && 'ion-invalid'}`}>
            <IonLabel>New Radius:</IonLabel>
            <IonInput type="number" defaultValue={minRadius} onIonInput={(event) => validateRadius(event)}></IonInput>
            <IonNote slot="error">Radius must be a number between {minRadius} and {maxRadius}</IonNote>
          </IonItem> 

          <IonItem className="submit-new-radius">
            <IonButton onClick={() => saveRadius()}>Save</IonButton>
          </IonItem>

      </IonContent>
    </IonPage>
  );
};

export default Fence;
