import styled from 'styled-components';

const ErrorContainer = styled.div`
  background: #252525;
  border: 3px solid #313131;
  border-radius: 15px 15px 0 0;
  padding: 20px 120px;
  margin-top: 20px;
  width: 100%;
`;

const ErrorTitle = styled.h2`
  color: #F3F3F3;
  font-size: 20px;
  margin: 0 0 16px 0;
`;

const ErrorMessage = styled.p`
  font-size: 20px;
  line-height: 32px;
  margin: 0;
  color: ${props => props.isError ? '#FF4F4F' : '#797979'};
  transition: color 0.3s ease-in-out;
`;

const ErrorMessages = ({ motorTemp, waterFlowRate }) => {
  return (
    <ErrorContainer>
      <ErrorTitle>Error message</ErrorTitle>
      <ErrorMessage isError={motorTemp > 40}>
        Motor temperature is too high.
      </ErrorMessage>
      <ErrorMessage isError={waterFlowRate > 115}>
        Water flow rate IN status is too high.
      </ErrorMessage>
    </ErrorContainer>
  );
};

export default ErrorMessages;